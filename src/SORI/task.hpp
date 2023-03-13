#pragma once
#include <string>
#include <map>
#include "dng/primitives.hpp"

namespace sori
{


class TaskContext
{
public:
    virtual bool isDone() const = 0;
    virtual void onClick(const dng::Point& pos) = 0;
    virtual int getScore() const = 0;
    virtual void draw(dng::Image& surface) const = 0;
    virtual ~TaskContext() {}
};

class ITask
{
public:
    virtual ~ITask() {}

    virtual std::string getName() const = 0;
    virtual std::unique_ptr<TaskContext> createContext(const dng::Size& envSize) const = 0;
    virtual int getSolvedScore() const = 0;
};

using TaskScores = std::map<std::string, int>;
class ITaskManager
{
public:
    virtual ~ITaskManager() {}

    virtual ITask& pickNextTask(const TaskScores& taskScores) = 0;
};

}
