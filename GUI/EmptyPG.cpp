#include "EmptyPG.hpp"

class EmptyFitnessEvaluator : public neat::IFitnessEvaluator
{
public:
   EmptyFitnessEvaluator()
   {
      
   }

   void step()
   {
      
   }

   neat::Fitness evaluate(const neat::Genom& g) override
   {
      return 0;
   }
};

neat::Config EmptyPG::getConfig()
{
   neat::Config c;

   c.numInputs = 2;
   c.numOutputs = 2;
   c.initialPopulation = 100;
   c.optimalPopulation = 100;
   c.compatibilityFactor = 3.0;
   c.inheritDisabledChance = 0.75;
   c.perturbationChance = 0.9;
   c.addNodeMutationChance = 0.05;
   c.addConnectionMutationChance = 0.05;
   c.removeConnectionMutationChance = 0.05;
   c.weightsMutationChance = 0.8;
   c.C1_C2 = 1.0;
   c.C3 = 0.3;
   c.startConnected = true;
   c.numThreads = 3;

   return c;
}

neat::IFitnessEvaluator& EmptyPG::getFitnessEvaluator()
{
   return *mFitnessEvaluator;
}

EmptyPG::EmptyPG()
: mFitnessEvaluator(new EmptyFitnessEvaluator)
{
   
}

void EmptyPG::step()
{
   
}

void EmptyPG::play(const neat::Genom& g)
{
   
}

std::string EmptyPG::getName() const
{
   return "Empty";
}