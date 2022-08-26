#pragma once

#include "pop.hpp"
#include "task.hpp"

namespace sori
{

class Environment
{
public:
    Environment(const Size& sz, const ITask& task, const std::size_t energyLimit);

    void run(Pop& pop);

private:
    Image mSurface;
    const ITask& mTask;
    const std::size_t mEnergyLimit;
};

}
