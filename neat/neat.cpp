#include "neat.hpp"
#include "rng.hpp"
#include <ctime>
#include <algorithm>
#include <numeric>

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
        mPopulation = createInitialPopulation(mCfg.numInputs, mCfg.numOutputs, mCfg.initialPopulation, mHistory);
    }
    else
    {
        mPopulation = nextGeneration(*mPopulation);
    }

    evaluateFitness();
}

void Neat::evaluateFitness()
{
    for(auto& s : (*mPopulation))
    {
        for(auto &p : s.population)
        {
            p.fitness = mFitnessEvaluator.evaluate(p.genotype);
        }
    }
}

Pop& randomPop(Specie& s)
{
    return s.population[Rng::genChoise(s.population.size())];
}

bool comparePopsByFitness(const Pop& a, const Pop& b)
{
   return a.fitness > b.fitness;
}

bool comparePopsByFitnessLess(const Pop& a, const Pop& b)
{
   return a.fitness < b.fitness;
}

bool isCompatible(const Genom& g1, const Genom& g2, const double compatibilityFactor)
{
    return Genom::calculateDivergence(g1, g2) < compatibilityFactor;
}

std::vector<unsigned int> Neat::getSpeciesOffspringQuotas(const Population& p)
{
    std::vector<Fitness> specieFitness = p.getSpeciesSharedFitness();
    Fitness totalFitness = std::accumulate(specieFitness.begin(), specieFitness.end(), 0);

    std::vector<unsigned int> numOffspringsPerSpecie;

    if(totalFitness == 0)
    {
        numOffspringsPerSpecie.push_back(mCfg.optimalPopulation / p.numSpecies());
    }
    else
    {
        for(std::size_t i = 0; i < specieFitness.size(); ++i)
        {
            numOffspringsPerSpecie.push_back(specieFitness[i] / double(totalFitness) * mCfg.optimalPopulation);
        }
    }
    

    return numOffspringsPerSpecie;
}

void Neat::reproduceAndMutate(Population& pops, std::vector<Genom>& out)
{
    std::vector<unsigned int> quotas = getSpeciesOffspringQuotas(pops);

    out.reserve(mCfg.optimalPopulation);

    //reproduce & mutate
    int specieNum = 0;
    for(auto& s : pops)
    {
        if(s.population.size() >= 5)
        {
            //Remove 20% of losers
            std::sort(s.population.begin(), s.population.end(), comparePopsByFitness);
            for(unsigned int i = 0; i < s.population.size() / 5; ++i)
            {
                s.population.pop_back();
            }

            //Keep champion
            out.push_back(s.population[0].genotype);
        }

        for(unsigned int i = 0; i < quotas[specieNum]; ++i)
        {
            auto& pop1 = randomPop(s);
            auto& pop2 = randomPop(s);

            auto genom = &pop1 == &pop2 ? pop1.genotype : Genom::crossover(pop1.genotype, pop2.genotype, pop1.fitness, pop2.fitness);
            mutate(genom, mHistory);

            out.push_back(genom);
        }

        specieNum++;
    }
}

Population Neat::createPopulationFromGenoms(const std::vector<Pop>& specieSamples, const std::vector<Genom>& newGenoms, Population& pops)
{
    Population result;

    auto maxSpecieId = std::max_element(pops.begin(), pops.end(), [] (auto x, auto y){return x.id < y.id;})->id;

    for(auto& g : newGenoms)
    {
        bool found = false;
        for(std::size_t i = 0; i < specieSamples.size(); ++i)
        {
            if(isCompatible(g, specieSamples[i].genotype, mCfg.compatibilityFactor))
            {
                result.instantiatePop(g, pops[i].id);
                found = true;
                break;
            }
        }

        if(!found)
        {
            auto id = result.instantiateSpecie(++maxSpecieId);
            result.instantiatePop(g, id);
        }
    }

    return result;
}

Population Neat::nextGeneration(Population& pops)
{
    //create list of same species and represent them with random pop of this specie
    std::vector<Pop> specieSamples = pops.createSpeciesSamples();
    
    std::vector<Genom> newGenoms;
    reproduceAndMutate(pops, newGenoms);

    return createPopulationFromGenoms(specieSamples, newGenoms, pops);
}

const Population& Neat::getPopulation() const
{
    return *mPopulation;
}

}