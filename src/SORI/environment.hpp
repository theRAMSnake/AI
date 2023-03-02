#pragma once

#include "pop.hpp"
#include "task.hpp"

namespace sori
{

class Environment
{
public:
    Environment(const dng::Size& sz, const ITask& task, const std::size_t energyLimit);

    void run(Pop& pop);

private:
    const dng::Size& mSize;
    const ITask& mTask;
    const std::size_t mEnergyLimit;
};

}
