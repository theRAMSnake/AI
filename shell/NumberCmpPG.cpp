#include "NumberCmpPG.hpp"
#include "neat/population.hpp"
#include "neat/neuro_net.hpp"
#include <boost/random.hpp>

boost::random::mt19937 rng;


class FitnessEvaluator : public neat::IFitnessEvaluator
{
public:
   FitnessEvaluator()
   {
      rng.seed(time(nullptr));

      step();
   }

   void step()
   {
      mValues.clear();

      mValues.reserve(2300);
      for(int i = 0; i < 2000; ++i)
      {
         mValues.push_back(rng() % 100);
      }
      for(int i = 0; i < 300; ++i)
      {
         auto val = rng() % 100;
         mValues.push_back(val);
         mValues.push_back(val);
      }
   }

   bool test(const neat::Genom& g, double val1, double val2)
   {
      auto n = neat::NeuroNet(g);
      auto a = n.activateLongTerm({static_cast<double>(val1), static_cast<double>(val2)});

      auto pos = std::max_element(a.begin(), a.end());
      if(pos == a.begin()) // >
      {
         return val1 > val2;
      }
      else if(pos == a.begin() + 1) // =
      {
         return val1 == val2;
      }
      else // <
      {
         return val1 < val2;
      }

      return false;
   }

   neat::Fitness evaluate(const neat::Genom& g) override
   {
      neat::Fitness result = 0;

      for(int i = 0; i < 2600; i += 2)
      {
         auto val1 = mValues[i];
         auto val2 = mValues[i + 1];

         if(test(g, val1, val2) && test(g, val2, val1))
         {
            result++;
         }
      }

      //return result;
      return 1;
   }

private:
   std::vector<int> mValues;
};

neat::Config NumberCmpPG::getConfig()
{
   neat::Config c;

   c.numInputs = 2;
   c.numOutputs = 3;  // >, =, <
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
   c.numThreads = 3;

   return c;
}

neat::IFitnessEvaluator& NumberCmpPG::getFitnessEvaluator()
{
   return *mFitnessEvaluator;
}

NumberCmpPG::NumberCmpPG()
: mFitnessEvaluator(new FitnessEvaluator)
{
   
}

void NumberCmpPG::step()
{
   mFitnessEvaluator->step();
}

void NumberCmpPG::play(const neat::Genom& g)
{
   auto n = neat::NeuroNet(g);

   while(true)
   {
      int num1;
      int num2;

      std::cout << "Enter 2 numbers: ";

      std::cin >> num1;
      std::cin >> num2;

      auto a = n.activateLongTerm({static_cast<double>(num1), static_cast<double>(num2)});

      auto pos = std::max_element(a.begin(), a.end());
      if(pos == a.begin()) // >
      {
         std::cout << num1 << '>' << num2 << std::endl;
      }
      else if(pos == a.begin() + 1) // =
      {
         std::cout << num1 << '=' << num2 << std::endl;
      }
      else // <
      {
         std::cout << num1 << '<' << num2 << std::endl;
      }
   }
}