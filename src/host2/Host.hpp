#pragma once

#include <vector>
#include <map>
#include <thread>
#include <boost/property_tree/ptree.hpp>
#include "IProject.hpp"

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
};

class Host
{
public:
    Host();
    ~Host();

    HostState getState() const;
    std::vector<std::string> listProjects() const;
    void createProject(const std::string& name);
    void loadProject(const std::string& name);
    void deleteProject(const std::string& name);

    void run(const StopCondition& condition);
    std::string play(const std::size_t popIdx, const std::string& taskName);
    void stop();

    void set(const std::string& key, const std::string& value);
    boost::property_tree::ptree getConfig() const;
    std::string printLatestStatistics() const;

    void exportPop(const std::size_t idx, const std::string& filename) const;

private:
    const std::string mRootDir;
    void threadFunc(const StopCondition condition);
    void saveProject();

    HostState mState = HostState::Empty;

    std::unique_ptr<IProject> mCurrentProject;
    std::string mCurrentProjectName;

    std::thread mRunnerThread;
    std::atomic<bool> mStopTriggered = false;
};
