#include "Host.hpp"
#include <filesystem>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "sori.hpp"

Host::Host()
    : mRootDir(getenv("HOME") + std::string("/.snakeai"))
{
    std::filesystem::create_directory(mRootDir);
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

    result.put("population", 100);
    result.put("autosave_period", 10);
    result.put("num_threads", std::thread::hardware_concurrency() / 2);

    return result;
}

void Host::createProject(const std::string& name)
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

    auto newCfg = makeDefaultProjectConfig();

    mCurrentProject = sori::createProject(newCfg);
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

    mCurrentProject = sori::loadProject(mRootDir + "/" + name + "/state", cfg);
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

std::string Host::printLatestStatistics() const
{
    if(mState == HostState::Empty)
    {
        throw std::runtime_error("Cannot print stats in this state");
    }

    return mCurrentProject->printRecentStats();
}

void Host::threadFunc(const StopCondition condition)
{
    auto startTime = std::chrono::system_clock::now();
    auto nextAutoSaveTime = std::chrono::system_clock::now() + std::chrono::minutes(mCurrentProject->getConfig().get<std::size_t>("autosave_period"));
    while( (condition.minutes == 0 || std::chrono::duration_cast<std::chrono::minutes>(std::chrono::system_clock::now() - startTime) < std::chrono::minutes(condition.minutes))
        && !mStopTriggered)
    {
        mCurrentProject->step();

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
