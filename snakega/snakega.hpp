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
};

class Algorithm
{
public:
   Algorithm(
      const Config& cfg,
      const neuroevolution::DomainGeometry& domainGeometry, 
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

   Config mCfg;
   neuroevolution::IFitnessEvaluator& mFitnessEvaluator;
   std::vector<Pop> mPopulation;

   neuroevolution::Fitness mBestFitness;
};

}