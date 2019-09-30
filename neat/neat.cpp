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
}

NeatResult Neat::step()
{
    if(!mPopulation)
    {
        mPopulation = createInitialPopulation(mCfg.numInputs, mCfg.numOutputs, mCfg.initialPopulation);
        mInnovationNumber = mPopulation->begin()->population[0].genotype.length() + 1;
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

Population Neat::nextGeneration(Population& pops)
{
    Population next;

    //create list of same species and represent them with random pop of this specie
    std::vector<Pop> specieSamples = pops.createSpeciesSamples();
    std::vector<Fitness> specieFitness = pops.getSpeciesSharedFitness();
    Fitness totalFitness = std::accumulate(specieFitness.begin(), specieFitness.end(), 0);

    //calculate num offsprings
    //HERE!
    std::vector<unsigned int> numOffspringsPerSpecie;

    for(std::size_t i = 0; i < specieFitness.size(); ++i)
    {
        numOffspringsPerSpecie.push_back(specieFitness[i] / double(totalFitness) * mCfg.optimalPopulation + 1);
    }

    //eliminate low fitness(remove worst 20%)
    for(auto& s : pops)
    {
        if(s.population.size() < 5)
        {
            continue;
        }
        else
        {
            std::sort(s.population.begin(), s.population.end(), comparePopsByFitness);
            for(unsigned int i = 0; i < s.population.size() / 5; ++i)
            {
                s.population.pop_back();
            }
        }
    }

    std::vector<Genom> newGenoms;
    newGenoms.reserve(mCfg.optimalPopulation);

    //reproduce & mutate
    int specieNum = 0;
    for(auto& s : pops)
    {
        for(unsigned int i = 0; i < numOffspringsPerSpecie[specieNum]; ++i)
        {
            auto& pop1 = randomPop(s);
            auto& pop2 = randomPop(s);

            auto genom = &pop1 == &pop2 ? pop1.genotype : Genom::crossover(pop1.genotype, pop2.genotype, pop1.fitness, pop2.fitness);
            mutate(genom, mInnovationNumber);

            newGenoms.push_back(genom);
        }

        specieNum++;
    }

    //keep champion of (5+ species)
    for(auto& s : pops)
    {
        if(s.population.size() < 5)
        {
            continue;
        }
        else
        {
            newGenoms.push_back(std::max_element(s.population.begin(), s.population.end(), comparePopsByFitnessLess)->genotype);
        }
    }

    //speciate
    for(auto& g : newGenoms)
    {
        bool found = false;
        for(std::size_t i = 0; i < specieSamples.size(); ++i)
        {
            if(isCompatible(g, specieSamples[i].genotype, mCfg.compatibilityFactor))
            {
                next.instantiatePop(g, pops[i].id);
                found = true;
                break;
            }
        }

        if(!found)
        {
            auto id = next.instantiateSpecie();
            next.instantiatePop(g, id);
        }
    }

    return next;
}

}