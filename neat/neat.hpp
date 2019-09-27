#pragma once
#include "population.hpp"
#include "genom.hpp"
#include <optional>

namespace neat
{

class NeatResult
{

};

class IFitnessEvaluator
{
public:
    virtual Fitness evaluate(const Genom& g) = 0;
};

struct Config
{
    NodeId numInputs;
    NodeId numOutputs;
    unsigned int initialPopulation;
    unsigned int optimalPopulation;
};

class Neat
{
public:
    Neat(const Config& cfg, IFitnessEvaluator& fitnessEvaluator);
    NeatResult step();

private:
    Population nextGeneration(Population& pops);
    void evaluateFitness();

    Config mCfg;
    IFitnessEvaluator& mFitnessEvaluator;

    std::optional<Population> mPopulation;
    InnovationNumber mInnovationNumber;
};

}