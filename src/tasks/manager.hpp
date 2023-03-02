#pragma once

#include "SORI/task.hpp"

namespace tlib
{

class TaskManager : public sori::ITaskManager
{
public:
    sori::ITask& pickNextTask(const sori::TaskScores& taskScores) override;
};

}

