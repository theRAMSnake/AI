#pragma once

namespace neat
{

class IEvolutionStrategy
{
public:
   virtual void setGenerationResults(const double fitness, const unsigned int averageComplexity) = 0;
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
   void setGenerationResults(const double fitness, const unsigned int averageComplexity) override
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
   void setGenerationResults(const double fitness, const unsigned int averageComplexity) override
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
            mSteps++;
            if(mSteps > GROWING_LIMIT)
            {
               mSteps = 0;
               mGrowing = false;
               mLowestComplexity = averageComplexity;
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
      return mGrowing ? "Growing: " + std::to_string(GROWING_LIMIT - mSteps) :  "Shrinking: " + std::to_string(SHRINKING_LIMIT - mSteps);
   }

private:
   bool mGrowing = true;
   unsigned int mSteps = 0;
   double mTopFitness = 0;
   unsigned int mLowestComplexity = 0;
   const unsigned int GROWING_LIMIT = 250;
   const unsigned int SHRINKING_LIMIT = 25;
};

}
