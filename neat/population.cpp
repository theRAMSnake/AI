#include "population.hpp"
#include <algorithm>

namespace neat
{

Population createInitialPopulation(const NodeId numInputs, const NodeId numOutputs, const unsigned int size)
{
   Population p;

   Specie s{0};

   for(unsigned int i = 0; i < size; ++i)
   {
      s.population.push_back({0, Genom::createMinimal(numInputs, numOutputs)});
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

}