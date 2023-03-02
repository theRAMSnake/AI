#include "manager.hpp"
#include "gacommon/rng.hpp"
#include "ObjectDetection.hpp"

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

    result.task = std::make_shared<Task<TContext>>(TContext::TASK_NAME, TContext::MAX_SCORE);
    result.prerequisites.push_back({prerequisiteTaskName, prerequisiteScore});

    return result;
}

template<class TContext>
TaskDefinition createDefinition()
{
    TaskDefinition result;

    result.task = std::make_shared<Task<TContext>>(TContext::TASK_NAME, TContext::MAX_SCORE);

    return result;
}

const std::vector<TaskDefinition> gAllTasks = {
    createDefinition<ObjectDetection1>(),
    createDefinition<ObjectDetection2>(),
    createDefinition<ObjectDetection3>(),
    createDefinition<ObjectDetection4>(),
    createDefinition<ObjectDetection5>()
};

sori::ITask& TaskManager::pickNextTask(const sori::TaskScores& taskScores)
{
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
        if(pos != taskScores.end() && pos->second >= randomCandidate.task->getMaxScore())
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
