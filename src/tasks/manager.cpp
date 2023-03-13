#include "manager.hpp"
#include "gacommon/rng.hpp"
#include "ObjectDetection.hpp"
#include <boost/gil/extension/io/bmp/write.hpp>

namespace tlib
{

struct Prerequisite
{
    std::string taskName;
    int score = 0;
};

struct TaskDefinition
{
    std::vector<Prerequisite> prerequisites;
    std::shared_ptr<sori::ITask> task;
};

template<class TContext>
TaskDefinition createDefinition(const std::string& prerequisiteTaskName, const int prerequisiteScore)
{
    TaskDefinition result;

    result.task = std::make_shared<Task<TContext>>(TContext::TASK_NAME, TContext::SOLVED_SCORE);
    result.prerequisites.push_back({prerequisiteTaskName, prerequisiteScore});

    return result;
}

template<class TContext>
TaskDefinition createDefinition()
{
    TaskDefinition result;

    result.task = std::make_shared<Task<TContext>>(TContext::TASK_NAME, TContext::SOLVED_SCORE);

    return result;
}

const std::vector<TaskDefinition> gAllTasks = {
    createDefinition<ObjectDetection1>(),
    createDefinition<ObjectDetection2>("ObjectDetection1", 950),
    createDefinition<ObjectDetection3>("ObjectDetection2", 950),
    createDefinition<ObjectDetection4>("ObjectDetection3", 950),
    createDefinition<ObjectDetection5>("ObjectDetection4", 950)
};

void TaskManager::dumpDemoPictures(const std::filesystem::path& dirName) const
{
    for(auto& d : gAllTasks)
    {
        dng::Image surface(500, 364);
        d.task->createContext({500, 364})->draw(surface);
        auto fileName = dirName / d.task->getName();
        boost::gil::write_view(fileName, boost::gil::view(surface), boost::gil::bmp_tag());
    }
}

sori::ITask& TaskManager::pickNextTask(const sori::TaskScores& taskScores)
{
    //Policy here is task is elegible if it has no prerequisites or at least one of the prerequisites is reached specified score
    //If task is maxed it got only 10% chance to be used
    std::vector<TaskDefinition> candidates;

    for(auto& d : gAllTasks)
    {
        for(auto& p : d.prerequisites)
        {
            auto pos = taskScores.find(p.taskName);
            if(pos != taskScores.end() && pos->second >= p.score)
            {
                candidates.push_back(d);
                break;
            }
        }

        if(d.prerequisites.empty())
        {
            candidates.push_back(d);
        }
    }

    while(true)
    {
        const auto& randomCandidate = candidates[Rng::genChoise(candidates.size())];
        auto pos = taskScores.find(randomCandidate.task->getName());
        if(pos != taskScores.end() && pos->second >= randomCandidate.task->getSolvedScore())
        {
            if(Rng::genProbability(0.1))
            {
                return *randomCandidate.task;
            }
        }
        else
        {
            return *randomCandidate.task;
        }
    }
}

}
