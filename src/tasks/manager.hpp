#pragma once

#include "SORI/task.hpp"
#include <filesystem>

namespace tlib
{

class TaskManager : public sori::ITaskManager
{
public:
    sori::ITask& pickNextTask(const sori::TaskScores& taskScores) override;

    void dumpDemoPictures(const std::filesystem::path& dirName) const;
};

}

