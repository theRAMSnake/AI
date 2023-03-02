#pragma once
#include "SORI/task.hpp"

namespace tlib
{

template<class TContext>
class Task : public sori::ITask
{
public:
    Task(const std::string& name, const int maxScore)
        : mName(name)
        , mMaxScore(maxScore)
    {
    }

    std::string getName() const override
    {
        return mName;
    }

    std::unique_ptr<sori::TaskContext> createContext(const dng::Size& envSize) const override
    {
        return std::make_unique<TContext>(envSize);
    }

    int getMaxScore() const override
    {
        return mMaxScore;
    }

private:
    const std::string mName;
    const int mMaxScore;
};

}
