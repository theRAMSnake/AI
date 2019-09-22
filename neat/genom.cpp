#include "genom.hpp"
#include <boost/random.hpp>

namespace neat
{

boost::random::mt19937 rng;
boost::uniform_real<> doubleGen(-1.0, 1.0);
boost::uniform_real<> perturbationGen(-0.05, 0.05);
boost::uniform_real<> realGen(0, 1.0);

const double INHERIT_DISABLED_CHANCE = 0.75;
const double PERTURBATION_CHANCE = 0.9;

Genom crossover(const Genom& a, const Genom& b, const Fitness fitA, const Fitness fitB)
{
    //Pick shared part till innovations match (pick random weight)
    //Pick excess part from most fit (or random in case of same fitness)
    //Gene MIGHT be disabled if it is disabled in one of the parents
    Genom g;

    const bool aIsFittier = fitA == fitB ? doubleGen(rng) > 0 : fitA > fitB;
    const Genom& fittest = aIsFittier ? a : b;

    g.genes.reserve(fittest.genes.size());

    std::size_t divergencePoint = 0;

    for(std::size_t i = 0; i < std::min(a.genes.size(), b.genes.size()); ++i)
    {
        if(a.genes[i].innovationNumber == b.genes[i].innovationNumber)
        {
            g.genes.push_back(a.genes[i]);
            g.genes.back().weight = doubleGen(rng) > 0 ? a.genes[i].weight : b.genes[i].weight;

            if(!a.genes[i].enabled || !b.genes[i].enabled)
            {
                if(realGen(rng) < INHERIT_DISABLED_CHANCE)
                {
                    g.genes.back().enabled = false;
                }
            }
        }
        else
        {
            divergencePoint = i;
            break;
        }
    }

    if(divergencePoint != 0)
    {
        g.genes.insert(g.genes.end(), fittest.genes.begin() + divergencePoint, fittest.genes.end());
    }

    return g;
}

Genom createMinimalGenom(const int numInputs, const int numOutputs)
{
    Genom g;

    g.genes.reserve(numInputs * numOutputs);

    InnovationNumber innovationNumber = 0;
    for(int i = 0; i < numInputs; ++i)
    {
        for(int j = 0; j < numOutputs; ++j)
        {
            g.genes.push_back({i, j + numInputs, true, innovationNumber++, doubleGen(rng)});
        }
    }
    return g;
}

void mutateWights(Genom& a)
{
    for(auto& x : a.genes)
    {
        if(realGen(rng) < PERTURBATION_CHANCE)
        {
            x.weight += perturbationGen(rng);
        }
        else
        {
            x.weight = doubleGen(rng);
        }
    }
}

void mutateAddConnection(Genom& a)
{
    
}

}

