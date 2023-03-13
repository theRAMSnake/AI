#pragma once
#include "SORI/task.hpp"

namespace tlib
{

template<class TContext>
class Task : public sori::ITask
{
public:
    Task(const std::string& name, const int solvedScore)
        : mName(name)
        , mSolvedScore(solvedScore)
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

    int getSolvedScore() const override
    {
        return mSolvedScore;
    }

private:
    const std::string mName;
    const int mSolvedScore;
};

}
