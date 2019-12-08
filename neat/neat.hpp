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
    void reconfigure(const Config& cfg);

    const Population& getPopulation() const;
    bool hasPopulation() const;

    void saveState(const std::string& fileName);
    void loadState(const std::string& fileName);

private:

    void updateFitness();

    Config mCfg;
    IFitnessEvaluator& mFitnessEvaluator;

    std::optional<Population> mPopulation;
    InnovationHistory mHistory;
};

}