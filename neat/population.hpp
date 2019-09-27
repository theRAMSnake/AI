#pragma once
#include "genom.hpp"

namespace neat
{

struct Pop
{
   Fitness fitness;
   Genom genotype;
};

struct Specie
{
   unsigned int id;
   std::vector<Pop> population;
   Fitness sharedFitness;
};

class Population
{
public:
   using Iterator = std::vector<Specie>::iterator;

   void operator += (const Specie& p);
   Iterator begin();
   Iterator end();

   void instantiatePop(const Genom& g, const unsigned int specieId);

private:

   std::vector<Specie> mSpecies;
};

Population createInitialPopulation(const NodeId numInputs, const NodeId numOutputs, const unsigned int size);

}