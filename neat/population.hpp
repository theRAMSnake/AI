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

   const Pop& randomPop() const;
};

class Population
{
public:
   using Iterator = std::vector<Specie>::iterator;

   void operator += (const Specie& p);
   Iterator begin();
   Iterator end();
   Specie& operator[] (const std::size_t index);
   std::size_t numSpecies() const;

   void instantiatePop(const Genom& g, const unsigned int specieId);
   unsigned int instantiateSpecie();
   std::vector<Pop> createSpeciesSamples() const;
   std::vector<Fitness> getSpeciesSharedFitness() const;

private:

   std::vector<Specie> mSpecies;
};

Population createInitialPopulation(const NodeId numInputs, const NodeId numOutputs, const unsigned int size);

}