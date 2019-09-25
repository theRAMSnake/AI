#pragma once
#include "genom.hpp"

namespace neat
{

struct Pop
{
   Fitness fitness;
   Genom genotype;
};

class Population
{
public:
   using Iterator = std::vector<Pop>::iterator;

   void operator += (const Pop& p);
   Iterator begin();
   Iterator end();

private:
   std::vector<Pop> mPops;
};

Population createInitialPopulation(const NodeId numInputs, const NodeId numOutputs, const unsigned int size);

}