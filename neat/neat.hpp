#pragma once
#include "population.hpp"
#include "genom.hpp"
#include "Substrate3D.hpp"
#include <optional>
#include "neuroevolution/IPlayground.hpp"

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
    Neat(const Config& cfg, const EvolutionStrategyType esType, neuroevolution::IFitnessEvaluator& fitnessEvaluator);
    Neat(
        const Config& cfg, 
        const neuroevolution::DomainGeometry& domainGeometry, 
        const EvolutionStrategyType esType, 
        neuroevolution::IFitnessEvaluator& fitnessEvaluator
        );
        
    void step();
    void reconfigure(const Config& cfg, const EvolutionStrategyType esType);

    const Population& getPopulation() const;
    bool hasPopulation() const;

    void saveState(const std::string& fileName);
    void loadState(const std::string& fileName);

    std::string getEsInfo() const;
    std::unique_ptr<neuroevolution::NeuroNet> createAnn(const v2::Genom& src) const;

private:

    void updateFitness();

    int evaluate(
        neuroevolution::IFitnessEvaluator* eval, 
        std::vector<std::vector<Pop>::iterator>::iterator begin, 
        std::vector<std::vector<Pop>::iterator>::iterator end
        );
    void evaluateParallel( std::vector<std::vector<Pop>::iterator>& popPtrs, neuroevolution::IFitnessEvaluator& eval, int numThreads);

    Config mCfg;
    neuroevolution::IFitnessEvaluator& mFitnessEvaluator;

    std::shared_ptr<IEvolutionStrategy> mEs;
    std::optional<Population> mPopulation;
    std::optional<Substrate3D> mSubstrate;
    InnovationHistory mHistory;
};

}