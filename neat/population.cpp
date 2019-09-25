#include "population.hpp"

namespace neat
{

Population createInitialPopulation(const NodeId numInputs, const NodeId numOutputs, const unsigned int size)
{
   Population p;

   for(unsigned int i = 0; i < size; ++i)
   {
      p += Pop{0, Genom::createMinimal(numInputs, numOutputs)};
   }

   return p;
}

void Population::operator += (const Pop& p)
{
   mPops.push_back(p);
}

Population::Iterator Population::begin()
{
   return mPops.begin();
}

Population::Iterator Population::end()
{
   return mPops.end();
}

}