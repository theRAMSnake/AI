#pragma once
#include "population.hpp"
#include "genom.hpp"
#include <optional>
#include "gacommon/IPlayground.hpp"

namespace neat
{

enum class EvolutionStrategyType
{
    Blend,
    Phasing
};

struct Config
{
    unsigned int numInputs;
    unsigned int numOutputs;
    unsigned int numThreads; 
    v2::MutationConfig mutationCfg;
    Population::Config populationCfg;
};

class Neat
{
public:
    Neat(const Config& cfg, const EvolutionStrategyType esType, gacommon::IFitnessEvaluator& fitnessEvaluator);
        
    void step();
    void reconfigure(const Config& cfg, const EvolutionStrategyType esType);

    const Population& getPopulation() const;
    bool hasPopulation() const;

    void saveState(const std::string& fileName);
    void loadState(const std::string& fileName);

    std::string getEsInfo() const;
    std::unique_ptr<gacommon::NeuroNet2> createAnn(const v2::Genom& src) const;

    std::size_t getGenerationNumber() const;

private:

    void updateFitness();

    int evaluate(
        gacommon::IFitnessEvaluator* eval, 
        std::vector<std::vector<Pop>::iterator>::iterator begin, 
        std::vector<std::vector<Pop>::iterator>::iterator end
        );
    void evaluateParallel( std::vector<std::vector<Pop>::iterator>& popPtrs, gacommon::IFitnessEvaluator& eval, int numThreads);

    Config mCfg;
    gacommon::IFitnessEvaluator& mFitnessEvaluator;

    std::shared_ptr<IEvolutionStrategy> mEs;
    std::optional<Population> mPopulation;
    InnovationHistory mHistory;
    std::size_t mGeneration = 1;
};

}
