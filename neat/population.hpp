#pragma once
#include "genom.hpp"

namespace neat
{

class IFitnessEvaluator
{
public:
    virtual Fitness evaluate(const Genom& g) = 0;

    virtual ~IFitnessEvaluator(){}
};

struct Pop
{
   Fitness fitness;
   Genom genotype;
};

struct Specie
{
   const Pop& randomPop() const;
   void selectRepresentor();
   Fitness getSharedFitness() const;
   Fitness getTotalFitness() const;
   void updateFitness();
   bool isStagnant() const;
   void produceOffsprings(const unsigned int amount, InnovationHistory& history, std::vector<Genom>& out);

   std::optional<Pop> representor;
   unsigned int id;
   std::vector<Pop> population;
   Fitness maxFitness;
   unsigned int numStagnantGenerations;
   Fitness totalFitness = 0;
   Fitness sharedFitness = 0;
};

class Population
{
public:
   Population(const unsigned int optimalSize, const double compatibilityFactor);

   using Iterator = std::vector<Specie>::iterator;
   using ConstIterator = std::vector<Specie>::const_iterator;

   ConstIterator begin() const;
   ConstIterator end() const;
   Iterator begin();
   Iterator end();
   const Specie& operator[] (const std::size_t index) const;
   std::size_t numSpecies() const;
   std::size_t size() const;

   Fitness getAverageFitness() const;

   void nextGeneration(InnovationHistory& history);
   void onEvaluationFinished();

   static Population createInitialPopulation(
      const NodeId numInputs, 
      const NodeId numOutputs, 
      const unsigned int size,
      const double compatibilityFactor,
      InnovationHistory& history
      );

private:
   std::vector<unsigned int> getSpeciesOffspringQuotas();
   unsigned int genNewSpecieId() const;

   const unsigned int mOptimalSize;
   const double mCompatibilityFactor;
   std::vector<Specie> mSpecies;
};
}