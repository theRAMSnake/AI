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

    virtual ~IFitnessEvaluator(){}
};

class Neat
{
public:
    Neat(const Config& cfg, IFitnessEvaluator& fitnessEvaluator);
    void step();

    const Population& getPopulation() const;

private:
    Population nextGeneration(Population& pops);
    void evaluateFitness();
    std::vector<unsigned int> getSpeciesOffspringQuotas(const Population& p);
    void reproduceAndMutate(Population& pops, std::vector<Genom>& out);
    Population createPopulationFromGenoms(
        const std::vector<Pop>& specieSamples, 
        const std::vector<Genom>& newGenoms, 
        Population& pops
        );

    Config mCfg;
    IFitnessEvaluator& mFitnessEvaluator;

    std::optional<Population> mPopulation;
    InnovationHistory mHistory;
};

}