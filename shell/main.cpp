#include "neat/neat.hpp"

#include <iostream>
#include <memory>
#include <boost/random.hpp>

std::unique_ptr<neat::Neat> current;
boost::random::mt19937 rng;

class FitnessEvaluator : public neat::IFitnessEvaluator
{
public:
   FitnessEvaluator()
   {
      rng.seed(time(nullptr));

      mValues.reserve(2000);
      for(int i = 0; i < 2000; ++i)
      {
         mValues.push_back(rng());
      }
   }

   neat::Fitness evaluate(const neat::Genom& g) override
   {
      neat::Fitness result = 0;

      /*auto n = neat::createNn(g);

      for(int i = 0; i < 2000; i += 2)
      {
         auto a = n({mValues[i], mValues[i + 1]});
         if(a[0] == mValues[i] + mValues[i + 1])
         {
            result++;
         }
      }*/

      return result;
   }

private:
   std::vector<int> mValues;
};

int main()
{
   std::cout << "Neat shell" << std::endl;

   FitnessEvaluator ev;
   neat::Config c;

   c.numInputs = 2;
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
   c.C3 = 2.0;

   current.reset(new neat::Neat(c, ev));
}