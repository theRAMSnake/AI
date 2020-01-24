#pragma once
#include "neuroevolution/IPlayground.hpp"
#include "pop.hpp"
#include "genom.hpp"
#include <memory>

namespace snakega
{
struct Config
{
   std::size_t populationSize;
   std::size_t championsKept;
   std::size_t exploitationDepth;
   std::size_t exploitationSize;
   double survivalRate;

   MutationConfig mutationConfig;

   std::size_t numThreads;
};

class Algorithm
{
public:
   Algorithm(
      const Config& cfg,
      const neuroevolution::DomainGeometry domainGeometry, 
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

   int exploitRange(
      std::vector<Pop>::iterator begin, 
      std::vector<Pop>::iterator end
      );

   void finalize();

   Config mCfg;
   const neuroevolution::DomainGeometry mDomainGeometry; 
   neuroevolution::IFitnessEvaluator& mFitnessEvaluator;
   std::vector<Pop> mPopulation;

   neuroevolution::Fitness mBestFitness;
};

}