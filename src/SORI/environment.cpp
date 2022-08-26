#include "environment.hpp"

namespace sori
{

Environment::Environment(const Size& sz, const ITask& task, const std::size_t energyLimit)
    : mSurface(static_cast<Image::coord_t>(sz.x), static_cast<Image::coord_t>(sz.y))
    , mTask(task)
    , mEnergyLimit(energyLimit)
{
    mTask.draw(mSurface);
}

void Environment::run(Pop& pop)
{
    pop.run(mEnergyLimit, mSurface, mTask.createContext());
}

}
