#pragma once
#include "genom.hpp"
#include <optional>
#include <memory>
#include "EvolutionStrategy.hpp"

namespace neat
{

struct Pop
{
   Fitness fitness;
   v2::Genom genotype;
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
      const v2::MutationConfig mutationCfg,
      std::vector<v2::Genom>& out
      );
   bool isStagnant() const;

   std::optional<Pop> representor;
   unsigned int id;
   std::vector<Pop> population;
   double maxFitness = 0.0;
   double maxAverageFitness = 0.0;
   unsigned int numStagnant = 0;
   Fitness totalFitness = 0;
   double sharedFitness = 0;
};

class Speciation
{
public:
   static void respeciate(
      std::vector<Specie>& species, 
      const std::vector<v2::Genom>& genoms, 
      const double compatibilityFactor,
      const double C1_C2,
      const double C3
      );

private:
   static unsigned int genNewSpecieId(const std::vector<Specie>& species);
};

class Population
{
public:
   struct Config
   {
      unsigned int size;
      double mCompatibilityFactor;
      double minterspecieCrossoverPercentage;
      double mC1_C2;
      double mC3;
   };

   Population( 
      const Config& config
      );

   void reconfigure(
      const Config& config
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
   double getAverageComplexity() const;

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
      const Config& config,
      InnovationHistory& history
      );

   void setEvolutionStrategy(std::shared_ptr<IEvolutionStrategy>& es);

private:
   std::vector<unsigned int> getSpeciesOffspringQuotas();

   double mAverageComplexity = 0.0;
   std::vector<Specie> mSpecies;

   Config mCfg;

   double mBestFitness = 0.0;
   double mMaxComplexity = 0.0;
   unsigned int mNumStagnantGenerations = 0;
   std::shared_ptr<IEvolutionStrategy> mEs;
};
}