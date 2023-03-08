#include "environment.hpp"

namespace sori
{

Environment::Environment(const dng::Size& sz, const ITask& task, const std::size_t energyLimit)
    : mSize(sz)
    , mTask(task)
    , mEnergyLimit(energyLimit)
{
}

void Environment::run(Pop& pop)
{
    auto ctx = mTask.createContext(mSize);
    //Limitation: static drawing only, fine for now
    dng::Image surface(mSize.x, mSize.y);
    ctx->draw(surface);
    pop.run(mEnergyLimit, surface, *ctx);
}

}
