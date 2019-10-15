#pragma once
#include "population.hpp"
#include "genom.hpp"
#include <optional>

namespace neat
{

class NeatResult
{

};

class Neat
{
public:
    Neat(const Config& cfg, IFitnessEvaluator& fitnessEvaluator);
    void step();

    const Population& getPopulation() const;

private:

    void updateFitness();

    Config mCfg;
    IFitnessEvaluator& mFitnessEvaluator;

    std::optional<Population> mPopulation;
    InnovationHistory mHistory;
};

}