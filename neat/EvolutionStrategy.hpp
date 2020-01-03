#pragma once
#include <string>

namespace neat
{

class IEvolutionStrategy
{
public:
   virtual void setGenerationResults(const double fitness, const double averageComplexity) = 0;
   virtual bool isCrossoverAllowed() const = 0;
   virtual bool enableDoomsday() const = 0;
   virtual v2::MutationConfig getAllowedMutations() const = 0;
   virtual std::string getInfo() const = 0;

   ~IEvolutionStrategy()
   {

   }
};

class BlendEvolutionStrategy : public IEvolutionStrategy
{
public:
   BlendEvolutionStrategy(const v2::MutationConfig& cfg)
   : mCfg(cfg)
   {

   }
   
   void setGenerationResults(const double fitness, const double averageComplexity) override
   {

   }

   bool isCrossoverAllowed() const override
   {
      return true;
   }

   v2::MutationConfig getAllowedMutations() const override
   {
      return mCfg;
   }

   std::string getInfo() const override
   {
      return "Blending";
   }

   bool enableDoomsday() const override
   {
       return false;
   }

private:
   const v2::MutationConfig& mCfg;
};

class PhasingEvolutionStrategy : public IEvolutionStrategy
{
public:
   PhasingEvolutionStrategy(const v2::MutationConfig& cfg)
   {
      mGrowingCfg = cfg;
      mGrowingCfg.removeConnectionMutationChance = 0;
      mGrowingCfg.removeNodeMutationChance = 0;

      mShrinkingCfg = cfg;
      mShrinkingCfg.addConnectionMutationChance = 0;
      mShrinkingCfg.addNodeMutationChance = 0;
   }

   void setGenerationResults(const double fitness, const double averageComplexity) override
   {
      if(mGrowing)
      {
         if(fitness > mTopFitness || fitness == 0)
         {
            mSteps = 0;
            mTopFitness = fitness;
         }
         else
         {
             if (averageComplexity > mComplexityTreshold)
             {
                 mSteps++;
                 if (mSteps > mGrowingLimit)
                 {
                     mSteps = 0;
                     mGrowing = false;
                     mLowestComplexity = averageComplexity;
                 }
             }
         }
      }
      else
      {
         if(averageComplexity < mLowestComplexity)
         {
            mSteps = 0;
            mLowestComplexity = averageComplexity;
         }
         else
         {
            mSteps++;
            if(mSteps > SHRINKING_LIMIT)
            {
               mSteps = 0;
               mGrowing = true;
               mComplexityTreshold = mLowestComplexity + 50;
               mTopFitness = fitness;
            }
         }
      }
   }

   bool isCrossoverAllowed() const override
   {
      return mGrowing;
   }

   v2::MutationConfig getAllowedMutations() const override
   {
      return mGrowing ? mGrowingCfg : mShrinkingCfg;
   }

   std::string getInfo() const override
   {
      return mGrowing ? "Growing: " + std::to_string(mGrowingLimit - mSteps) :  "Shrinking: " + std::to_string(SHRINKING_LIMIT - mSteps);
   }

   bool enableDoomsday() const override
   {
       return false;
   }

private:
   bool mGrowing = true;
   unsigned int mSteps = 0;
   double mTopFitness = 0;
   double mLowestComplexity = 0;
   unsigned int mComplexityTreshold = 0;
   unsigned int mGrowingLimit = 25;
   const unsigned int SHRINKING_LIMIT = 15;

   v2::MutationConfig mGrowingCfg;
   v2::MutationConfig mShrinkingCfg;
};

}
