#pragma once
#include "genom.hpp"
#include <optional>
#include <memory>
#include "EvolutionStrategy.hpp"

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
   void produceOffsprings(
      const unsigned int amount, 
      InnovationHistory& history, 
      const bool isCrossoverAllowed,
      const Mutation allowedMutations,
      std::vector<Genom>& out
      );

   std::optional<Pop> representor;
   unsigned int id;
   std::vector<Pop> population;
   double maxFitness;
   Fitness totalFitness = 0;
   double sharedFitness = 0;
};

class Speciation
{
public:
   static void respeciate(std::vector<Specie>& species, const std::vector<Genom>& genoms, const double compatibilityFactor);

private:
   static unsigned int genNewSpecieId(const std::vector<Specie>& species);
};

class Population
{
public:
   Population(
      const unsigned int optimalSize, 
      const double compatibilityFactor, 
      const double interspecieCrossoverPercentage
      );

   void reconfigure(
      const unsigned int optimalSize, 
      const double compatibilityFactor, 
      const double interspecieCrossoverPercentage
      );

   using Iterator = std::vector<Specie>::iterator;
   using ConstIterator = std::vector<Specie>::const_iterator;

   ConstIterator begin() const;
   ConstIterator end() const;
   Iterator begin();
   Iterator end();
   const Specie& operator[] (const std::size_t index) const;
   std::size_t numSpecies() const;
   std::size_t size() const;

   double getAverageFitness() const;

   void nextGeneration(InnovationHistory& history);
   void onEvaluationFinished();

   void saveState(std::ofstream& s);
   void loadState(
      std::ifstream& s, 
      InnovationHistory& history, 
      const NodeId numInputs, 
      const NodeId numOutputs
      );

   static Population createInitialPopulation(
      const NodeId numInputs, 
      const NodeId numOutputs, 
      const unsigned int size,
      const double compatibilityFactor,
      const double interspecieCrossoverPercentage,
      InnovationHistory& history
      );

   void setEvolutionStrategy(std::shared_ptr<IEvolutionStrategy>& es);

private:
   std::vector<unsigned int> getSpeciesOffspringQuotas();

   unsigned int mOptimalSize;
   double mCompatibilityFactor;
   double minterspecieCrossoverPercentage;
   std::vector<Specie> mSpecies;

   std::shared_ptr<IEvolutionStrategy> mEs;
};
}