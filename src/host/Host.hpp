#pragma once

#include <vector>
#include <map>
#include <thread>
#include "gacommon/IPlayground.hpp"
#include "Engines.hpp"

enum class HostState
{
    Empty,
    Ready,
    Running,
    Stopping
};

struct StopCondition
{
    std::size_t minutes;
    gacommon::Fitness fitness;
};

struct PreviewSnapshot
{
    gacommon::Fitness bestFitness = 0;
    std::size_t generation = 0;
    std::chrono::seconds lastGenerationTime;
    std::chrono::minutes totalTime;
};

class Host
{
public:
    Host();
    ~Host();

    HostState getState() const;
    std::vector<std::string> listProjects() const;
    void createProject(const std::string& name, const std::string& engine, const std::string& playgroundName);
    void loadProject(const std::string& name);
    void deleteProject(const std::string& name);

    void run(const StopCondition& condition);
    void stop();

    PreviewSnapshot getLastSnapshot() const;
    std::vector<PopInfo> describePopulation() const;
    std::vector<std::byte> exportIndividual(const PopId& id) const;
    std::string perform(const PopId& id) const;

    void set(const std::string& key, const std::string& value);
    boost::property_tree::ptree getConfig() const;

private:
    const std::string mRootDir;
    void threadFunc(const StopCondition condition);
    void saveProject();

    HostState mState = HostState::Empty;
    std::map<std::string, std::unique_ptr<IEngine>> mEngines;

    std::unique_ptr<IProject> mCurrentProject;
    std::string mCurrentProjectName;

    std::thread mRunnerThread;
    std::atomic<bool> mStopTriggered = false;
    PreviewSnapshot mLastSnapshot;
    mutable std::mutex mMutex;
};
