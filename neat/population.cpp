#include "population.hpp"
#include "rng.hpp"
#include <algorithm>
#include <numeric>

namespace neat
{

Population createInitialPopulation(
   const NodeId numInputs, 
   const NodeId numOutputs, 
   const unsigned int size,
   InnovationHistory& history
   )
{
   Population p;

   Specie s{0};

   for(unsigned int i = 0; i < size; ++i)
   {
      s.population.push_back({0, Genom::createMinimal(numInputs, numOutputs, history)});
   }

   return p;
}

void Population::operator += (const Specie& s)
{
   mSpecies.push_back(s);
}

Population::Iterator Population::begin()
{
   return Population::Iterator(mSpecies.begin());
}

Population::Iterator Population::end()
{
   return Population::Iterator(mSpecies.end());
}

void Population::instantiatePop(const Genom& g, const unsigned int specieId)
{
   auto speciePos = std::find_if(mSpecies.begin(), mSpecies.end(), [=](auto x){return x.id == specieId;});
   if(speciePos != mSpecies.end())
   {
      speciePos->population.push_back({0, g});
   }
   else
   {
      mSpecies.push_back(Specie{specieId, {Pop{0, g}}});
   }
}

std::vector<Pop> Population::createSpeciesSamples() const
{
   std::vector<Pop> result;

   result.reserve(mSpecies.size());

   for(auto& s : mSpecies)
   {
      result.push_back(s.randomPop());
   }

   return result;
}

std::vector<Fitness> Population::getSpeciesSharedFitness() const
{
   std::vector<Fitness> result;

   result.reserve(mSpecies.size());

   for(auto& s : mSpecies)
   {
      auto totalFitness = std::accumulate(s.population.begin(), s.population.end(), 0, [](auto a, auto b){return a + b.fitness;}) ;
      result.push_back(totalFitness / s.population.size());
   }

   return result;
}

Specie& Population::operator[] (const std::size_t index)
{
   return mSpecies[index];
}

unsigned int Population::instantiateSpecie()
{
   auto newId = std::max_element(mSpecies.begin(), mSpecies.end(), [](auto a, auto b){return a.id < b.id;})->id + 1;
   mSpecies.push_back({newId, {}});

   return newId;
}

std::size_t Population::numSpecies() const
{
   return mSpecies.size();
}

const Pop& Specie::randomPop() const
{
    return population[Rng::genChoise(population.size())];
}

}