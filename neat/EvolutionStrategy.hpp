#pragma once
#include <string>

namespace neat
{

class IEvolutionStrategy
{
public:
   virtual void setGenerationResults(const double fitness, const double averageComplexity) = 0;
   virtual bool isCrossoverAllowed() const = 0;
   virtual Mutation getAllowedMutations() const = 0;
   virtual std::string getInfo() const = 0;

   ~IEvolutionStrategy()
   {

   }
};

class BlendEvolutionStrategy : public IEvolutionStrategy
{
public:
   void setGenerationResults(const double fitness, const double averageComplexity) override
   {

   }

   bool isCrossoverAllowed() const override
   {
      return true;
   }

   Mutation getAllowedMutations() const override
   {
      return Mutation::All;
   }

   std::string getInfo() const override
   {
      return "Blending";
   }
};

class PhasingEvolutionStrategy : public IEvolutionStrategy
{
public:
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

   Mutation getAllowedMutations() const override
   {
      return mGrowing ? Mutation::Complexifying : Mutation::Simplifying;
   }

   std::string getInfo() const override
   {
      return mGrowing ? "Growing: " + std::to_string(mGrowingLimit - mSteps) :  "Shrinking: " + std::to_string(SHRINKING_LIMIT - mSteps);
   }

private:
   bool mGrowing = true;
   unsigned int mSteps = 0;
   double mTopFitness = 0;
   double mLowestComplexity = 0;
   unsigned int mComplexityTreshold = 0;
   unsigned int mGrowingLimit = 25;
   const unsigned int SHRINKING_LIMIT = 15;
};

}
