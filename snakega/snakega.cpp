#include "snakega.hpp"
#include "genom.hpp"
#include "exploitation.hpp"
#include "fitness_weighted_pool.hpp"
#include "neuroevolution/rng.hpp"
#include <algorithm>
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
   , mDomainGeometry(domainGeometry)
   , mFitnessEvaluator(fitnessEvaluator)
{
   for(std::size_t i = 0; i < mCfg.populationSize; ++i)
   {
      //mPopulation.push_back(Genom::createHalfConnected(domainGeometry.inputs.size(), domainGeometry.outputs.size()));
      mPopulation.push_back(Genom::createGeometrical(domainGeometry));
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
    LOG_FUNC
   //Note: can be changed by more effective CMA-ES
   for(auto iter = begin; iter != end; ++iter)
   {
      Exploitation exploitation(*iter, mCfg.exploitationSize);

      *iter = exploitation.run(mCfg.exploitationDepth, mCfg.mutationConfig, mDomainGeometry, mFitnessEvaluator);
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
   //Quick for now
   std::ofstream ofile(fileName, std::ios::binary | std::ios::trunc);

   auto size = mPopulation.size();
   ofile.write((char*)&size, sizeof(std::size_t));

   for(auto& p : mPopulation)
   {
      ofile.write((char*)&p.mFitness, sizeof(neuroevolution::Fitness));
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
      Pop p(Genom(mDomainGeometry.inputs.size(), mDomainGeometry.outputs.size()));

      ifile.read((char*)&p.mFitness, sizeof(neuroevolution::Fitness));
      p.mGenom = Genom::loadState(ifile, mDomainGeometry.inputs.size(), mDomainGeometry.outputs.size());

      mPopulation.push_back(p);
   }
}

}

