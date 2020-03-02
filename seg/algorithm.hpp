#pragma once
#include "neuroevolution/IPlayground.hpp"
#include "mutator.hpp"

namespace seg
{

struct Config
{
   std::size_t populationSize;
   std::size_t championsKept;
   double survivalRate;

   MutationConfig mutationConfig;

   std::size_t numThreads;
   std::size_t memorySize;
};

struct Pop
{
    neuroevolution::Fitness fitness;
    Graph graph;
};


class Algorithm
{
public:
   Algorithm(
      const Config& cfg,
      const unsigned int numInputs,
      const unsigned int numOutputs,
      neuroevolution::IFitnessEvaluator& fitnessEvaluator
      );

   void step();
   void reconfigure(const Config& cfg);

   const std::vector<Pop>& getPopulation() const;

   void saveState(const std::string& fileName);
   void loadState(const std::string& fileName);

private:
   std::vector<Pop> select() const;
   void repopulate(const std::vector<Pop>& pops);
   void exploit();
   void finalize();

   int exploitRange(
      std::vector<Pop>::iterator begin, 
      std::vector<Pop>::iterator end
      );

   Config mCfg;
   neuroevolution::IFitnessEvaluator& mFitnessEvaluator;
   std::vector<Pop> mPopulation;
   neuroevolution::Fitness mBestFitness = 0;
   const unsigned int mNumInputs;
   const unsigned int mNumOutputs;
};

}