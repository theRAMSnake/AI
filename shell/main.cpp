#include "neat/neat.hpp"
#include "neat/neuro_net.hpp"

#include <iostream>
#include <memory>
#include <chrono>
#include <boost/random.hpp>

std::unique_ptr<neat::Neat> current;
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

      return result;
   }

private:
   std::vector<int> mValues;
};

void printGenom(const neat::Genom& g)
{
   std::cout << "         (";

   for(auto& x : g)
   {
      if(x.enabled)
      {
         std::cout << x.srcNodeId << "->" << x.dstNodeId << " ";
      }
   }

   std::cout << ")" << std::endl;
}

int main()
{
   std::cout << "Neat shell" << std::endl;

   FitnessEvaluator ev;
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

   current.reset(new neat::Neat(c, ev));

   int generation = 0;
   char buf[300];
   while(std::cin.getline(buf, 300))
   {
      auto t1 = std::chrono::high_resolution_clock::now();
      std::string s = buf;

      if(s == "")
      {
         ev.step();
         current->step();

         generation++;
      }
      else if(s == "+")
      {
         std::cout << "generations: ";
         int g = 0;
         std::cin >> g;

         for(int i = 0; i < g; ++i)
         {
            ev.step();
            current->step();

            generation++;

            std::cout << ".";
            std::cout.flush();
         }

         std::cin.get();
      }
      else if(s == ">")
      {
         std::cout << "value: ";
         neat::Fitness f = 0;
         std::cin >> f;
         
         bool stop = false;
         while(!stop)
         {
            ev.step();
            current->step();

            generation++;

            auto& pops = current->getPopulation();
            for(auto& s : pops)
            {
               auto best = std::max_element(s.population.begin(), s.population.end(), [](auto x, auto y){return x.fitness < y.fitness;});
               if(best->fitness >= f)
               {
                  stop = true;
                  break;
               }
            }

            std::cout << ".";
            std::cout.flush();
         }

         std::cin.get();
      }
      else if(s == "q")
      {
         return 0;
      }

      auto& pops = current->getPopulation();
      std::cout << "-----------------------------------------------------------" << std::endl;

      auto t2 = std::chrono::high_resolution_clock::now();

      auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();

      std::cout << "Executed: " << duration << "ms" << std::endl;

      std::cout << "Generation: " << generation << std::endl;
      std::cout << "Total population: " << pops.size() << std::endl;
      std::cout << "Num species: " << pops.numSpecies() << std::endl;
      std::cout << "Average fitness: " << pops.getAverageFitness() << std::endl;
      std::cout << "Best of each specie: " << std::endl;

      for(auto& s : pops)
      {
         auto best = std::max_element(s.population.begin(), s.population.end(), [](auto x, auto y){return x.fitness < y.fitness;});
         std::cout << s.id << ": Fitness: " << best->fitness << std::endl;
         printGenom(best->genotype);
      }
   }
}