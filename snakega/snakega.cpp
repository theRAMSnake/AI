#include "snakega.hpp"
#include "genom.hpp"
#include "exploitation.hpp"
#include "neuroevolution/rng.hpp"
#include <algorithm>
#include <numeric>
#include <future>
#include "neuroevolution/BinaryIO.hpp"
#include <iostream>
#include "logger/Logger.hpp"

namespace snakega
{

Algorithm::Algorithm(
   const Config& cfg,
   const neuroevolution::DomainGeometry domainGeometry, 
   neuroevolution::IFitnessEvaluator& fitnessEvaluator
   )
   : mCfg(cfg)
   , mFitnessEvaluator(fitnessEvaluator)
   , mNumInputs(domainGeometry.inputs.size())
   , mNumOutputs(domainGeometry.outputs.size())
   , mBestFitness(0.0)
{
   mPopulation.reserve(mCfg.populationSize);
   for(std::size_t i = 0; i < mCfg.populationSize; ++i)
   {
      mPopulation.push_back(Genom::createMinimal(mNumInputs, mNumOutputs));
   }
}

void Algorithm::step()
{
   repopulate(select());
   exploit();
   finalize();
}

std::vector<Pop> Algorithm::select()
{
   //Keep champions, and *survivalRate* selected randomly, weighted by fitness. Never pick same guy
   std::vector<Pop> result;
   result.reserve(mCfg.populationSize);

   if(mBestFitness != 0)//Keep champions
   {
      std::copy(mPopulation.begin(), mPopulation.begin() + mCfg.championsKept, std::back_inserter(result));
   }
   
   while(result.size() < mCfg.populationSize * mCfg.survivalRate)
   {
      result.push_back(selectTournament(mPopulation));
   }

   return result;
}

int Algorithm::exploitRange(
   std::vector<Pop>::iterator begin, 
   std::vector<Pop>::iterator end
   )
{
   for(auto iter = begin; iter != end; ++iter)
   {
      Exploitation exploitation(*iter, mCfg.exploitationSize, mCfg.exploitationDepth);

      *iter = exploitation.run(mFitnessEvaluator);
   }

   return 0;
}

void Algorithm::exploit()
{
   std::vector<std::future<int>> fs;
   std::size_t numElementsByThread = mPopulation.size() / mCfg.numThreads + 1;

   auto first = mPopulation.begin();
   auto last = first + numElementsByThread;
   if(last > mPopulation.end())
   {
      last = mPopulation.end();
   }

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

void Algorithm::repopulate(const std::vector<Pop>& selected)
{
   mPopulation.clear();

   std::copy(selected.begin(), selected.end(), std::back_inserter(mPopulation));
   auto needOffsprings = mCfg.populationSize - selected.size();
   auto totalFitness = std::accumulate(mPopulation.begin(), mPopulation.end(), 0, [](auto a, auto p) {return a + p.fitness; });

   if (totalFitness != 0)
   {
      for (std::size_t i = 0; i < selected.size(); ++i)
      {
         auto& p = selected[i];
         if(i < mCfg.championsKept)
         {
            mPopulation.push_back(p);
         }

         auto numOffspring = std::max(1, (int)(double(p.fitness) / totalFitness * needOffsprings));
         for (int j = 0; j < numOffspring; ++j)
         {
            auto pop = p;
            pop.mutateStructure();

            mPopulation.push_back(pop);
         }
      }
   }

   while (mPopulation.size() < mCfg.populationSize)
   {
      auto pop = selectTournament(selected);
      pop.mutateStructure();

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
   neuroevolution::BinaryOutput out(fileName);

   out << mPopulation;
}

void Algorithm::loadState(const std::string& fileName)
{
   neuroevolution::BinaryInput in(fileName);

   in >> mPopulation;

   mBestFitness = mPopulation[0].fitness;
}

}

