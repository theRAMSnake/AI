#include "snakega.hpp"
#include "genom.hpp"
#include "exploitation.hpp"
#include "fitness_weighted_pool.hpp"
#include "neuroevolution/rng.hpp"
#include <algorithm>
#include <future>

namespace snakega
{

Algorithm::Algorithm(
   const Config& cfg,
   const neuroevolution::DomainGeometry domainGeometry, 
   neuroevolution::IFitnessEvaluator& fitnessEvaluator
   )
   : mCfg(cfg)
   , mDomainGeometry(domainGeometry)
   , mFitnessEvaluator(fitnessEvaluator)
{
   for(std::size_t i = 0; i < mCfg.populationSize; ++i)
   {
      mPopulation.push_back(Genom::createHalfConnected(domainGeometry.inputs.size(), domainGeometry.outputs.size()));
   }
}

void Algorithm::step()
{
   repopulate(select());
   exploit();
   finalize();
}

std::vector<Pop> Algorithm::select() const
{
   //Keep champions, and *survivalRate* selected randomly, weighted by fitness. Never pick same guy
   std::vector<Pop> result;

   std::copy(mPopulation.begin(), mPopulation.begin() + mCfg.championsKept, std::back_inserter(result));
   FitnessWeightedPool pool(mPopulation.begin() + mCfg.championsKept, mPopulation.end(), mBestFitness);
   
   while(result.size() < mCfg.populationSize * mCfg.survivalRate)
   {
      result.push_back(pool.pick());
   }

   return result;
}

int Algorithm::exploitRange(
   std::vector<Pop>::iterator begin, 
   std::vector<Pop>::iterator end
   )
{
   //Note: can be changed by more effective CMA-ES
   for(auto iter = begin; iter != end; ++iter)
   {
      Exploitation exploitation(*iter, mCfg.exploitationSize);

      auto bestPop = exploitation.run(mCfg.exploitationDepth, mCfg.mutationConfig, mDomainGeometry, mFitnessEvaluator);
      if(bestPop.mFitness > iter->mFitness)
      {
         *iter = bestPop;
      }
   }

   return 0;
}

void Algorithm::exploit()
{
   std::vector<std::future<int>> fs;
   std::size_t numElementsByThread = mPopulation.size() / mCfg.numThreads + 1;

   auto first = mPopulation.begin();
   auto last = first + numElementsByThread;
   for(std::size_t i = 0; i < mCfg.numThreads; ++i)
   {
      auto f = std::async(
         std::launch::async, 
         std::bind(&Algorithm::exploitRange, this, std::placeholders::_1, std::placeholders::_2), 
         first, 
         last
         );
      fs.push_back(std::move(f));

      first += numElementsByThread;
      last += numElementsByThread;

      if(last > mPopulation.end())
      {
         last = mPopulation.end();
      }
   }
   
   for(auto& f : fs)
   {
      f.get();
   }
}

void Algorithm::repopulate(const std::vector<Pop>& pops)
{
   mPopulation.clear();

   std::copy(pops.begin(), pops.end(), std::back_inserter(mPopulation));

   while(mPopulation.size() < mCfg.populationSize)
   {
      auto pop = pops[Rng::genChoise(pops.size())];
      pop.mutateStructure(mCfg.mutationConfig);

      mPopulation.push_back(pop);
   }
}

static bool comparePopsByFitness(const Pop& a, const Pop& b)
{
   return a.mFitness > b.mFitness;
}

void Algorithm::finalize()
{
   std::sort(mPopulation.begin(), mPopulation.end(), comparePopsByFitness);
   mBestFitness = mPopulation[0].mFitness;
}

void Algorithm::reconfigure(const Config& cfg)
{
   mCfg = cfg;
}

const std::vector<Pop>& Algorithm::getPopulation() const
{
   return mPopulation;
}

void Algorithm::saveState(const std::string& fileName)
{
   //throw -1;
}

void Algorithm::loadState(const std::string& fileName)
{
   throw -1;
}

}

