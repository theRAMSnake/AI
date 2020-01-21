#pragma once
#include <vector>
#include "pop.hpp"
#include "neuroevolution/rng.hpp"

namespace snakega
{

class FitnessWeightedPool
{
public:
   FitnessWeightedPool(std::vector<Pop>::const_iterator begin, std::vector<Pop>::const_iterator end, neuroevolution::Fitness bestFitness)
   : mBestFitness(bestFitness)
   {
      for(auto iter = begin; iter != end; ++iter)
      {
         mPool.push_back(iter);
      }
   }

   const Pop& pick()
   {
      if(mPool.empty())
      {
         throw -1;
      }

      unsigned int attempts = 0;
      while(true)
      {
         auto choise = mPool.begin() + Rng::genChoise(mPool.size());
         if(mBestFitness != 0 && attempts++ < mPool.size() * 3) //Skip fitness check if we tried to many times - looks like only shity pops left,
                                                                // so it is irrelevant what to pick
         {
            if(!Rng::genProbability(double((*choise)->mFitness) / mBestFitness))
            {
               continue;
            }
         }

         mPool.erase(choise);
         return **choise;
      }
   }

private:
   neuroevolution::Fitness mBestFitness;
   std::vector<std::vector<Pop>::const_iterator> mPool;
};

}