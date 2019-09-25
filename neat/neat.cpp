#include "neat.hpp"
#include "rng.hpp"
#include <ctime>

namespace neat
{

Neat::Neat(const Config& cfg, IFitnessEvaluator& fitnessEvaluator)
: mCfg(cfg)
, mFitnessEvaluator(fitnessEvaluator)
{
    Rng::seed(static_cast<unsigned int>(std::time(0)));
}

NeatResult Neat::step()
{
    if(!mPopulation)
    {
        mPopulation = createInitialPopulation(mCfg.numInputs, mCfg.numOutputs, mCfg.initialPopulation);
    }
    else
    {
        mPopulation = nextGeneration(*mPopulation);
    }

    evaluateFitness();

    return NeatResult();
}

void Neat::evaluateFitness()
{
    for(auto& p : (*mPopulation))
    {
        p.fitness = mFitnessEvaluator.evaluate(p.genotype);
    }
}

Population Neat::nextGeneration(const Population& pops)
{
    throw -1;
}

}