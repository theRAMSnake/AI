#include "NeatPopulation.hpp"
#include <iostream>

double NeatPopulation::getAverageFitness() const
{
    return mSrc.hasPopulation() ? mSrc.getPopulation().getAverageFitness() : 0;
}

double NeatPopulation::getAverageComplexity() const
{
    return mSrc.hasPopulation() ? mSrc.getPopulation().getAverageComplexity() : 0;
}

std::size_t NeatPopulation::size() const
{
    return mSrc.hasPopulation() ? mSrc.getPopulation().size() : 0;
}

unsigned int NeatPopulation::getNumSpecies() const
{
    return mSrc.hasPopulation() ? mSrc.getPopulation().numSpecies() : 0;
}

std::vector<SpecieResults> NeatPopulation::getSpecies() const
{
    if(!mSrc.hasPopulation())
    {
        return {};
    }
    std::vector<SpecieResults> results;
    results.reserve(size());

    for(auto& s : mSrc.getPopulation())
    {
        results.push_back({s.id, s.getSharedFitness(), s.maxFitness});

        unsigned int i = 0;
        for(auto& p : s.population)
        {
            results.back().popResults.push_back(PopResult{s.id, i++, p.fitness, p.genotype.getComplexity(), p.genotype.getNodeCount(
                neat::v2::Genom::NodeType::Hidden
            )});
        }
    }

    return results;
}

std::unique_ptr<neuroevolution::IAgent> NeatPopulation::createAgent(const PopResult& pop) const
{
    auto iter = std::find_if(mSrc.getPopulation().begin(), mSrc.getPopulation().end(), [&](auto x) {return x.id == pop.specieId; });
    auto& genotype = iter->population[pop.organismId].genotype;
    return std::make_unique<neuroevolution::NNAgent>(
        genotype.getNodeCount(neat::v2::Genom::NodeType::Input),
        genotype.getNodeCount(neat::v2::Genom::NodeType::Output),
        mSrc.createAnn(genotype));
}

NeatPopulation::NeatPopulation(const neat::Neat& src)
: mSrc(src)
{

}

double NeatPopulation::getTopFitness() const
{
    if (!mSrc.hasPopulation())
    {
        return 0;
    }
    return std::max_element(mSrc.getPopulation().begin(), mSrc.getPopulation().end(), [](auto x, auto y) {return x.maxFitness < y.maxFitness; })->maxFitness;
}