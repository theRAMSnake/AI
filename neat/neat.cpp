#include "neat.hpp"
#include "rng.hpp"
#include <ctime>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace neat
{

Neat::Neat(const Config& cfg, IFitnessEvaluator& fitnessEvaluator)
: mCfg(cfg)
, mFitnessEvaluator(fitnessEvaluator)
{
    Rng::seed(static_cast<unsigned int>(std::time(0)));
    Genom::setConfig(cfg);
}

void Neat::step()
{
    if(!mPopulation)
    {
        mPopulation.emplace(Population::createInitialPopulation(
            mCfg.numInputs, 
            mCfg.numOutputs, 
            mCfg.initialPopulation,
            mCfg.compatibilityFactor, 
            mHistory));
    }
    else
    {
        mPopulation->nextGeneration(mHistory);
    }

    mPopulation->updateFitness(mFitnessEvaluator);
}

Pop& randomPop(Specie& s)
{
    auto totalFitness = std::accumulate(s.population.begin(), s.population.end(), 0, [](auto a, auto b){return a + b.fitness;});

    auto* p = &s.population[Rng::genChoise(s.population.size())];
    while(!Rng::genProbability((double)p->fitness / totalFitness))
    {
        p = &s.population[Rng::genChoise(s.population.size())];
    }

    return *p;
}

bool comparePopsByFitnessLess(const Pop& a, const Pop& b)
{
   return a.fitness < b.fitness;
}

const Population& Neat::getPopulation() const
{
    return *mPopulation;
}

}