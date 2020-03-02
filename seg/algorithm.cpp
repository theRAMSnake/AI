#include "algorithm.hpp"
#include "agent.hpp"
#include "snakega/fitness_weighted_pool.hpp"
#include "neuroevolution/rng.hpp"
#include <algorithm>
#include <numeric>
#include <future>
#include <fstream>
#include "logger/Logger.hpp"

namespace seg
{

Algorithm::Algorithm(
   const Config& cfg,
   const unsigned int numInputs,
   const unsigned int numOutputs,
   neuroevolution::IFitnessEvaluator& fitnessEvaluator
   )
   : mCfg(cfg)
   , mFitnessEvaluator(fitnessEvaluator)
   , mNumInputs(numInputs)
   , mNumOutputs(numOutputs)
{
   for(std::size_t i = 0; i < mCfg.populationSize; ++i)
   {
      mPopulation.push_back({0, Graph(numOutputs)});
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
   LOG_FUNC
   //Keep champions, and *survivalRate* selected randomly, weighted by fitness. Never pick same guy
   std::vector<Pop> result;

   std::size_t numChampionsKept = mBestFitness == 0 ? 0 : mCfg.championsKept;

   std::copy(mPopulation.begin(), mPopulation.begin() + numChampionsKept, std::back_inserter(result));
   snakega::FitnessWeightedPool<Pop> pool(mPopulation.begin() + numChampionsKept, mPopulation.end(), mBestFitness);
   
   while(result.size() < mCfg.populationSize * mCfg.survivalRate && !pool.empty())
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
   LOG_FUNC
   for(auto iter = begin; iter != end; ++iter)
   {
      Agent a(iter->graph, mCfg.memorySize);
      iter->fitness = mFitnessEvaluator.evaluate(a);
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
   LOG_FUNC
   mPopulation.clear();

   std::copy(pops.begin(), pops.end(), std::back_inserter(mPopulation));
   auto needOffspring = mCfg.populationSize - pops.size();
   auto totalFitness = std::accumulate(mPopulation.begin(), mPopulation.end(), 0, [](auto a, auto p) {return a + p.fitness; });

   if (totalFitness != 0)
   {
       for (auto& p : pops)
       {
           auto numOffspring = std::max(1, (int)(double(p.fitness) / totalFitness * needOffspring));
           for (int i = 0; i < numOffspring; ++i)
           {
               auto pop = p;

               Mutator m(mCfg.mutationConfig, pop.graph, mCfg.memorySize, mNumInputs, mNumOutputs);
               m.mutate();

               mPopulation.push_back(pop);
           }
       }
   }

    while (mPopulation.size() < mCfg.populationSize)
    {
        auto pop = pops[Rng::genChoise(pops.size())];
        Mutator m(mCfg.mutationConfig, pop.graph, mCfg.memorySize, mNumInputs, mNumOutputs);
        m.mutate();

        mPopulation.push_back(pop);
    }
}

static bool comparePopsByFitness(const Pop& a, const Pop& b)
{
   return a.fitness > b.fitness;
}

void Algorithm::finalize()
{
   std::sort(mPopulation.begin(), mPopulation.end(), comparePopsByFitness);
   mBestFitness = mPopulation[0].fitness;
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
   
}

void Algorithm::loadState(const std::string& fileName)
{
   throw -1;
}

}

 