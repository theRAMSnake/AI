#include "snake4.hpp"
#include "gacommon/rng.hpp"
#include <algorithm>
#include <numeric>
#include <future>
#include <iostream>
#include "logger/Logger.hpp"

namespace snake4
{

Algorithm::Algorithm(
   const gacommon::Config& cfg,
   const gacommon::IODefinition& io,
   gacommon::IFitnessEvaluator& fitnessEvaluator
   )
   : mImpl(cfg, io, fitnessEvaluator)
{
}

void Algorithm::step()
{
    mImpl.step();
}

void Algorithm::reconfigure(const gacommon::Config& cfg)
{
   mImpl.reconfigure(cfg);
}

const std::vector<Pop>& Algorithm::getPopulation() const
{
    return mImpl.getPopulation();
}

void Algorithm::saveState(const std::string& fileName)
{
    mImpl.saveState(fileName);
}

void Algorithm::loadState(const std::string& fileName)
{
    mImpl.loadState(fileName);
}

std::size_t Algorithm::getGenerationNumber() const
{
    return mImpl.getGenerationNumber();
}

}

