#pragma once
#include <vector>
#include "neuroevolution/IPlayground.hpp"
#include "neuroevolution/rng.hpp"

namespace snakega
{

template<class TPop>
class FitnessWeightedPool
{
public:
   FitnessWeightedPool(typename std::vector<TPop>::const_iterator begin, typename std::vector<TPop>::const_iterator end, neuroevolution::Fitness bestFitness)
   : mBestFitness(bestFitness)
   {
      for(auto iter = begin; iter != end; ++iter)
      {
         mPool.push_back(iter);
      }

      mMaxAttermpts = mPool.size() * 5;
   }

   const TPop& pick()
   {
      if(mPool.empty())
      {
         throw -1;
      }

      unsigned int attempts = 0;
      while(true)
      {
         auto choise = mPool.begin() + Rng::genChoise(mPool.size());
         if(mBestFitness != 0 && attempts++ < mMaxAttermpts) //Skip fitness check if we tried to many times - looks like only shity pops left,
                                                                // so it is irrelevant what to pick
         {
            if(!Rng::genProbability(double((*choise)->fitness) / mBestFitness))
            {
               continue;
            }
         }

         auto& result = **choise;
         mPool.erase(choise);
         return result;
      }
   }

   bool empty()
   {
       return mPool.empty();
   }

private:
   unsigned int mMaxAttermpts;
   neuroevolution::Fitness mBestFitness;
   typename std::vector<typename std::vector<TPop>::const_iterator> mPool;
};

}