#include "MaxPG.hpp"
#include "neat/population.hpp"
#include "neat/neuro_net.hpp"
#include <boost/random.hpp>

extern boost::random::mt19937 rng;

class FitnessEvaluator1 : public neat::IFitnessEvaluator
{
public:
   FitnessEvaluator1()
   {
      rng.seed(time(nullptr));

      step();
   }

   void step()
   {
      mValues.clear();

      mValues.resize(1000);
      for(int i = 0; i < 1000; ++i)
      {
         for(unsigned int j = 0; j < 10; ++j)
         {
            mValues[i].push_back(rng() % 100);
         }
      }
   }

   neat::Fitness test(const neat::Genom& g, const std::vector<unsigned int>& input)
   {
      auto n = neat::NeuroNet(g);
      std::vector<double> inputs;

      for(auto& x : input)
      {
         inputs.push_back((double)x);
      }

      auto result = neat::activate(n, inputs);

      auto max = *std::max_element(input.begin(), input.end());
      return static_cast<int>(max) == static_cast<int>(result[0]);
   }

   neat::Fitness evaluate(const neat::Genom& g) override
   {
      neat::Fitness result = 0;

      for(unsigned int i = 0; i < mValues.size(); i ++)
      {
         result += test(g, mValues[i]);
      }

      return result;
   }

private:
   std::vector<std::vector<unsigned int>> mValues;
};

neat::Config MaxPG::getConfig()
{
   neat::Config c;

   c.numInputs = 10;
   c.numOutputs = 1;
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
   c.C3 = 1.0;

   return c;
}

neat::IFitnessEvaluator& MaxPG::getFitnessEvaluator()
{
   return *mFitnessEvaluator;
}

MaxPG::MaxPG()
: mFitnessEvaluator(new FitnessEvaluator1)
{
   
}

void MaxPG::step()
{
   mFitnessEvaluator->step();
}

void MaxPG::play(const neat::Genom& g)
{

}