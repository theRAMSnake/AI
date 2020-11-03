#include "snakega.hpp"
#include "genom.hpp"
#include "exploitation.hpp"
#include "neuroevolution/rng.hpp"
#include <algorithm>
#include <numeric>
#include <future>
#include <fstream>
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

std::vector<Pop> Algorithm::select() const
{
   //Keep champions, and *survivalRate* selected randomly, weighted by fitness. Never pick same guy
   std::vector<Pop> result;
   result.reserve(mCfg.populationSize);

   std::size_t numChampionsKept = mBestFitness == 0 ? 0 : mCfg.championsKept;

   std::copy(mPopulation.begin(), mPopulation.begin() + numChampionsKept, std::back_inserter(result));
   //TODO: replace with tournament
   //FitnessWeightedPool<Pop> pool(mPopulation.begin() + numChampionsKept, mPopulation.end(), mBestFitness);
   
   //while(result.size() < mCfg.populationSize * mCfg.survivalRate && !pool.empty())
   {
      //result.push_back(pool.pick());
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
      std::size_t numChampionsKept = mBestFitness == 0 ? 0 : mCfg.championsKept;
      for (std::size_t i = 0; i < selected.size(); ++i)
      {
         auto& p = selected[i];
         if(i < numChampionsKept)
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
      auto pop = selected[Rng::genChoise(selected.size())];
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
   //Quick for now
   std::ofstream ofile(fileName, std::ios::binary | std::ios::trunc);

   auto size = mPopulation.size();
   ofile.write((char*)&size, sizeof(std::size_t));

   for(auto& p : mPopulation)
   {
      ofile.write((char*)&p.fitness, sizeof(neuroevolution::Fitness));
      p.mGenom.saveState(ofile);
   }
}

void Algorithm::loadState(const std::string& fileName)
{
   //Quick for now
   std::ifstream ifile(fileName, std::ios::binary);

   std::size_t size = 0;
   ifile.read((char*)&size, sizeof(std::size_t));

   mPopulation.clear();
   mPopulation.reserve(size);

   for(std::size_t i = 0; i < size; ++i)
   {
      Pop p(Genom(mNumInputs, mNumOutputs));

      ifile.read((char*)&p.fitness, sizeof(neuroevolution::Fitness));
      p.mGenom = Genom::loadState(ifile, mNumInputs, mNumInputs);

      mPopulation.push_back(p);
   }

   mBestFitness = mPopulation[0].fitness;
}

}

