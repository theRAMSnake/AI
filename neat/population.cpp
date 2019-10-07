#include "population.hpp"
#include "rng.hpp"
#include <algorithm>
#include <numeric>

namespace neat
{

Population Population::createInitialPopulation(
   const NodeId numInputs, 
   const NodeId numOutputs, 
   const unsigned int size, 
   const double compatibilityFactor,
   InnovationHistory& history
   )
{
   Population p(size, compatibilityFactor);

   Specie s;
   s.id = 0;
   s.maxFitness = 0;
   s.numStagnantGenerations = 0;

   for(unsigned int i = 0; i < size; ++i)
   {
      s.population.push_back({0, Genom::createMinimal(numInputs, numOutputs, history)});
   }

   p.mSpecies.push_back(s);

   return p;
}

Fitness Specie::getSharedFitness() const
{
   auto totalFitness = std::accumulate(population.begin(), population.end(), 0, [](auto a, auto b){return a + b.fitness;});
   return totalFitness / population.size();
}

void Specie::evaluate(IFitnessEvaluator& eval)
{
   for(auto &p : population)
   {
      p.fitness = eval.evaluate(p.genotype);
   }

   auto f = getSharedFitness();
   if(f > maxFitness)
   {
      numStagnantGenerations = 0;
      maxFitness = f;
   }
   else
   {
      numStagnantGenerations++;
   }
}

void Population::updateFitness(IFitnessEvaluator& eval)
{
   for(auto& s : mSpecies)
   {
      s.evaluate(eval);
   }
}

void Specie::selectRepresentor()
{
   representor = randomPop();
}

Population::Population(const unsigned int optimalSize, const double compatibilityFactor)
: mOptimalSize(optimalSize)
, mCompatibilityFactor(compatibilityFactor)
{

}

std::vector<unsigned int> Population::getSpeciesOffspringQuotas()
{
   Fitness totalFitness = std::accumulate(mSpecies.begin(), mSpecies.end(), 0, [](auto a, auto& s)
   {
      return s.getSharedFitness() + a;
   });

   std::vector<unsigned int> numOffspringsPerSpecie;

   if(totalFitness == 0)
   {
      for(std::size_t i = 0; i < mSpecies.size(); ++i)
      {
         numOffspringsPerSpecie.push_back(mOptimalSize / mSpecies.size());
      }
   }
   else
   {
      for(std::size_t i = 0; i < mSpecies.size(); ++i)
      {
         if(mSpecies[i].isStagnant())
         {
            numOffspringsPerSpecie.push_back(0);
         }
         numOffspringsPerSpecie.push_back(mSpecies[i].getSharedFitness() / double(totalFitness) * mOptimalSize);
      }
   }

   return numOffspringsPerSpecie;
}

bool comparePopsByFitness(const Pop& a, const Pop& b)
{
   return a.fitness > b.fitness;
}

void Specie::produceOffsprings(const unsigned int amount, InnovationHistory& history, std::vector<Genom>& out)
{
   if(population.empty())
   {
      return;
   }

   std::sort(population.begin(), population.end(), comparePopsByFitness);
   if(population.size() >= 5)
   {
      //Keep champion
      out.push_back(population[0].genotype);
   }
   
   while(population.size() > amount)
   {
      population.pop_back();
   }

   for(unsigned int i = 0; i < amount; ++i)
   {
      auto& pop1 = randomPop();

      if(Rng::genProbability(0.5) || population.size() == 1)
      {
            auto genom = pop1.genotype;
            mutate(genom, history);
            out.push_back(genom);
      }
      else
      {
            auto* pop2 = &pop1;
            while(pop2 == &pop1)
            {
               pop2 = &randomPop();
            }
            
            auto genom = Genom::crossover(pop1.genotype, pop2->genotype, pop1.fitness, pop2->fitness);
            //mutate(genom, mHistory);
            out.push_back(genom);
      }
   }
}

bool isCompatible(const Genom& g1, const Genom& g2, const double compatibilityFactor)
{
    return Genom::calculateDivergence(g1, g2) < compatibilityFactor;
}

unsigned int Population::genNewSpecieId() const
{
   return std::max_element(mSpecies.begin(), mSpecies.end(), [] (auto x, auto y){return x.id < y.id;})->id + 1;
}

void Population::nextGeneration(InnovationHistory& history)
{
   std::vector<Genom> newGenoms;

   for(auto& s : mSpecies)
   {
      s.selectRepresentor();
   }
   
   std::vector<unsigned int> quotas = getSpeciesOffspringQuotas();

   newGenoms.reserve(mOptimalSize);

   int specieNum = 0;
   for(auto& s : mSpecies)
   {
      s.produceOffsprings(quotas[specieNum], history, newGenoms);
      s.population.clear();
      specieNum++;
   }

   for(auto& g : newGenoms)
   {
      bool found = false;

      for(auto& s : mSpecies)
      {
         if(isCompatible(g, s.representor->genotype, mCompatibilityFactor))
         {
            s.population.push_back({0, g});
            found = true;
            break;
         }
      }

      if(!found)
      {
         Pop p {0, g};
         Specie s;
         s.id = genNewSpecieId();
         s.maxFitness = 0;
         s.representor = p;
         s.numStagnantGenerations = 0;
         s.population.push_back(p);
         mSpecies.push_back(s);
      }
   }

   //remove extinct
   mSpecies.erase(std::remove_if(mSpecies.begin(), mSpecies.end(), [](auto x){return x.population.empty();}), mSpecies.end());
}

const Specie& Population::operator[] (const std::size_t index) const
{
   return mSpecies[index];
}

std::size_t Population::numSpecies() const
{
   return mSpecies.size();
}

const Pop& Specie::randomPop() const
{
   auto totalFitness = std::accumulate(population.begin(), population.end(), 0, [](auto a, auto b){return a + b.fitness;});

   auto* p = &population[Rng::genChoise(population.size())];
   while(!Rng::genProbability(((double)p->fitness + 0.1) / totalFitness))
   {
      p = &population[Rng::genChoise(population.size())];
   }

   return *p;
}

std::size_t Population::size() const
{
   std::size_t result = 0;

   for(auto& x : mSpecies)
   {
      result += x.population.size();
   }

   return result;
}

Fitness Population::getAverageFitness() const
{
   Fitness totalFitness = std::accumulate(mSpecies.begin(), mSpecies.end(), 0, [](auto t, auto& s)
   {
      return t + s.getSharedFitness();
   });

   return totalFitness / mSpecies.size();
}

Population::ConstIterator Population::begin() const
{
   return mSpecies.begin();
}

Population::ConstIterator Population::end() const
{
   return mSpecies.end();
}

bool Specie::isStagnant() const
{
   return numStagnantGenerations >= 10;
}

}