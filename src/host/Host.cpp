#include "Host.hpp"
#include "Playgrounds.hpp"
#include <filesystem>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

Host::Host()
    : mRootDir(getenv("HOME") + std::string("/.snakeai"))
{
    std::filesystem::create_directory(mRootDir);

    mEngines["neat"] = std::make_unique<NeatEngine>();
    mEngines["snake4"] = std::make_unique<Snake4Engine>();
}

HostState Host::getState() const
{
    return mState;
}

std::vector<std::string> Host::listProjects() const
{
    std::vector<std::string> result;
    for (auto const& dir_entry : std::filesystem::directory_iterator{mRootDir})
    {
        result.push_back(dir_entry.path().filename());
    }

    return result;
}

boost::property_tree::ptree makeDefaultProjectConfig()
{
    boost::property_tree::ptree result;

    result.put("population", 500);
    result.put("autosave_period", 10);
    result.put("num_threads", std::thread::hardware_concurrency() / 2);

    return result;
}

void Host::createProject(const std::string& name, const std::string& engine, const std::string& playgroundName)
{
    if(mState != HostState::Empty && mState != HostState::Ready)
    {
        throw std::runtime_error("Cannot create a project in this state");
    }

    auto projects = listProjects();
    if(std::find(projects.begin(), projects.end(), name) != projects.end())
    {
        throw std::runtime_error("Project with name " + name + " already exists");
    }

    auto engPos = mEngines.find(engine);
    if(engPos == mEngines.end())
    {
        throw std::runtime_error("Engine with name " + name + " does not exists");
    }

    auto newCfg = makeDefaultProjectConfig();
    newCfg.put("engine", engine);
    newCfg.put("playground", playgroundName);

    mCurrentProject = engPos->second->createProject(createPlayground(playgroundName), newCfg);
    mCurrentProjectName = name;
    std::filesystem::create_directory(mRootDir + "/" + name);
    saveProject();
    mState = HostState::Ready;
}

boost::property_tree::ptree readProjectConfig(const std::string& path)
{
    boost::property_tree::ptree tree;
    boost::property_tree::read_json(path, tree);
    return tree;
}

void Host::loadProject(const std::string& name)
{
    if(mState != HostState::Empty && mState != HostState::Ready)
    {
        throw std::runtime_error("Cannot load a project in this state");
    }

    auto projects = listProjects();
    if(std::find(projects.begin(), projects.end(), name) == projects.end())
    {
        throw std::runtime_error("Project with name " + name + " does not exists");
    }

    auto cfg = readProjectConfig(mRootDir + "/" + name + "/cfg");
    auto engPos = mEngines.find(cfg.get<std::string>("engine"));
    if(engPos == mEngines.end())
    {
        throw std::runtime_error("Engine with name " + name + " does not exists");
    }

    mCurrentProject = engPos->second->loadProject(createPlayground(cfg.get<std::string>("playground")), cfg, mRootDir + "/" + name + "/state");
    mCurrentProjectName = name;
    mState = HostState::Ready;
}

void Host::deleteProject(const std::string& name)
{
    if(mState != HostState::Empty && mState != HostState::Ready)
    {
        throw std::runtime_error("Cannot delete a project in this state");
    }

    if(mCurrentProjectName == name)
    {
        mState = HostState::Empty;
        mCurrentProject.reset();
    }

    std::filesystem::remove_all(mRootDir + "/" + name);
}

Host::~Host()
{
    if(mRunnerThread.joinable())
    {
        mRunnerThread.join();
    }
}

void Host::threadFunc(const StopCondition condition)
{
    gacommon::Fitness bestFitness = 0;
    auto startTime = std::chrono::system_clock::now();
    auto nextAutoSaveTime = std::chrono::system_clock::now() + std::chrono::minutes(mCurrentProject->getConfig().get<std::size_t>("autosave_period"));
    while((condition.fitness == 0 || bestFitness < condition.fitness) &&
        (condition.minutes == 0 || std::chrono::duration_cast<std::chrono::minutes>(std::chrono::system_clock::now() - startTime) < std::chrono::minutes(condition.minutes))
        && !mStopTriggered)
    {
        auto generationTime = std::chrono::system_clock::now();
        mCurrentProject->step();

        PreviewSnapshot newSnapshot {
            mCurrentProject->getBestFitness(),
            mCurrentProject->getGenerationNumber(),
            std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - generationTime),
            std::chrono::duration_cast<std::chrono::minutes>(std::chrono::system_clock::now() - startTime)
        };
        bestFitness = newSnapshot.bestFitness;
        {
            std::lock_guard<std::mutex> guard(mMutex);
            mLastSnapshot = newSnapshot;
        }

        if(mStopTriggered || std::chrono::system_clock::now() > nextAutoSaveTime)
        {
            saveProject();
            nextAutoSaveTime = std::chrono::system_clock::now() + std::chrono::minutes(mCurrentProject->getConfig().get<std::size_t>("autosave_period"));
        }
    }

    mStopTriggered = false;
    mState = HostState::Ready;
}

void Host::run(const StopCondition& condition)
{
    if(mState != HostState::Ready)
    {
        throw std::runtime_error("Cannot run in this state");
    }

    if(mRunnerThread.joinable())
    {
        mRunnerThread.join();
    }

    mState = HostState::Running;
    mRunnerThread = std::thread(&Host::threadFunc, this, condition);
}

void Host::stop()
{
    if(mState != HostState::Running)
    {
        throw std::runtime_error("Cannot run in this state");
    }
    mStopTriggered = true;
}

PreviewSnapshot Host::getLastSnapshot() const
{
    if(mState == HostState::Empty)
    {
        throw std::runtime_error("Cannot run in this state");
    }
    std::lock_guard<std::mutex> guard(mMutex);
    return mLastSnapshot;
}

std::vector<PopInfo> Host::describePopulation() const
{
    if(mState != HostState::Ready)
    {
        throw std::runtime_error("Cannot run in this state");
    }

    return mCurrentProject->describePopulation();
}

std::vector<std::byte> Host::exportIndividual(const PopId& id) const
{
    if(mState != HostState::Ready)
    {
        throw std::runtime_error("Cannot run in this state");
    }

    return mCurrentProject->exportIndividual(id);
}

std::string Host::perform(const PopId& id) const
{
    if(mState != HostState::Ready)
    {
        throw std::runtime_error("Cannot run in this state");
    }

    return mCurrentProject->perform(id);
}

void Host::set(const std::string& key, const std::string& value)
{
    if(mState != HostState::Ready)
    {
        throw std::runtime_error("Cannot run in this state");
    }

    auto cfg = mCurrentProject->getConfig();
    cfg.put(key, value);
    mCurrentProject->reconfigure(cfg);
    saveProject();
}

boost::property_tree::ptree Host::getConfig() const
{
    if(mState == HostState::Empty)
    {
        throw std::runtime_error("Cannot run in this state");
    }

    return mCurrentProject->getConfig();
}

void Host::saveProject()
{
    auto cfg = mCurrentProject->getConfig();
    boost::property_tree::write_json(mRootDir + "/" + mCurrentProjectName + "/cfg", cfg);
    mCurrentProject->saveState(mRootDir + "/" + mCurrentProjectName + "/state");
}
