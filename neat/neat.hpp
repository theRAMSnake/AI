#pragma once
#include "population.hpp"
#include "genom.hpp"
#include <optional>

namespace neat
{

enum class EvolutionStrategyType
{
    Blend,
    Phasing
};

class Neat
{
public:
    Neat(const Config& cfg, const EvolutionStrategyType esType, IFitnessEvaluator& fitnessEvaluator);
    void step();
    void rebase();
    void reconfigure(const Config& cfg, const EvolutionStrategyType esType);

    const Population& getPopulation() const;
    bool hasPopulation() const;

    void saveState(const std::string& fileName);
    void loadState(const std::string& fileName);

    std::string getEsInfo() const;

private:

    void updateFitness();

    Config mCfg;
    IFitnessEvaluator& mFitnessEvaluator;

    std::shared_ptr<IEvolutionStrategy> mEs;
    std::optional<Population> mPopulation;
    InnovationHistory mHistory;
};

}