#include "population.hpp"
#include "rng.hpp"
#include <algorithm>
#include <numeric>
#include <iostream>

namespace neat
{

bool comparePopsByFitness(const Pop& a, const Pop& b)
{
   return a.fitness > b.fitness;
}

Population Population::createInitialPopulation(
   const NodeId numInputs, 
   const NodeId numOutputs, 
   const unsigned int size, 
   const double compatibilityFactor,
   const double interspecieCrossoverPercentage,
   InnovationHistory& history
   )
{
   Population p(size, compatibilityFactor, interspecieCrossoverPercentage);

   Specie s;
   s.id = 0;
   s.maxFitness = 0;

   for(unsigned int i = 0; i < size; ++i)
   {
      s.population.push_back({0, Genom::createMinimal(numInputs, numOutputs, history, false)});
   }

   p.mSpecies.push_back(s);

   return p;
}

Fitness Specie::getSharedFitness() const
{
   return sharedFitness;
}

void Population::onEvaluationFinished()
{
   for(auto& s : mSpecies)
   {
      s.updateFitness();
   }
}

void Specie::updateFitness()
{
   std::sort(population.begin(), population.end(), comparePopsByFitness);
   totalFitness = std::accumulate(population.begin(), population.end(), 0, [](auto a, auto b){return a + b.fitness;});
   sharedFitness = (double)totalFitness / population.size();

   maxFitness = population[0].fitness;
}

void Specie::selectRepresentor()
{
   representor = randomPop();
}

Population::Population(const unsigned int optimalSize, const double compatibilityFactor, const double interspecieCrossoverPercentage)
: mOptimalSize(optimalSize)
, mCompatibilityFactor(compatibilityFactor)
, minterspecieCrossoverPercentage(interspecieCrossoverPercentage)
{

}

std::vector<unsigned int> Population::getSpeciesOffspringQuotas()
{
   Fitness totalFitness = std::accumulate(mSpecies.begin(), mSpecies.end(), 0, [](auto a, auto& s)
   {
      return s.getTotalFitness() + a;
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
         numOffspringsPerSpecie.push_back(mSpecies[i].getTotalFitness() / double(getAverageFitness() + 0.001));
      }
   }

   return numOffspringsPerSpecie;
}



Fitness Specie::getTotalFitness() const
{
   return totalFitness;
}

void Specie::produceOffsprings(
   const unsigned int amount, 
   InnovationHistory& history, 
   const bool isCrossoverAllowed,
   const Mutation allowedMutations,
   std::vector<Genom>& out
   )
{
   if(population.empty() || amount == 0)
   {
      return;
   }

   unsigned int amountLeft = amount;
   if(population.size() >= 5)
   {
      //Keep champion
      out.push_back(population[0].genotype);
      amountLeft--;
   }

   auto halfSize = population.size() / 2;
   while (population.size() > 1 && population.size() > halfSize)//Keep at least one organisms
   {
       population.pop_back();
   }

   for(unsigned int i = 0; i < amountLeft; ++i)
   {
      auto& pop1 = randomPop();

      if(!isCrossoverAllowed || Rng::genProbability(0.5) || population.size() == 1)
      {
            auto genom = pop1.genotype;
            mutate(genom, history, static_cast<int>(allowedMutations));
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
            mutate(genom, history, static_cast<int>(allowedMutations));
            out.push_back(genom);
      }
   }
}

bool isCompatible(const Genom& g1, const Genom& g2, const double compatibilityFactor)
{
    return Genom::calculateDivergence(g1, g2) < compatibilityFactor;
}

unsigned int Speciation::genNewSpecieId(const std::vector<Specie>& species)
{
   return std::max_element(species.begin(), species.end(), [] (auto x, auto y){return x.id < y.id;})->id + 1;
}

void Population::reconfigure(const unsigned int optimalSize, const double compatibilityFactor, const double interspecieCrossoverPercentage)
{
   mOptimalSize = optimalSize;
   mCompatibilityFactor = compatibilityFactor;
   minterspecieCrossoverPercentage = interspecieCrossoverPercentage;
}

void Speciation::respeciate(std::vector<Specie>& species, const std::vector<Genom>& genoms, const double compatibilityFactor)
{
   for(auto& s : species)
   {
      s.population.clear();
   }

   for(auto& g : genoms)
   {
      Specie* mostCompatibleSpecie = nullptr;
      double bestDistance = compatibilityFactor;
      for(auto& s : species)
      {
         auto distance = Genom::calculateDivergence(g, s.representor->genotype);
         if(distance < bestDistance)
         {
            mostCompatibleSpecie = &s;
            bestDistance = distance;
         }
      }

      if(mostCompatibleSpecie == nullptr)
      {
         Pop p {0, g};
         Specie s;
         s.id = genNewSpecieId(species);
         s.maxFitness = 0;
         s.representor = p;
         s.population.push_back(p);
         species.push_back(s);
      }
      else
      {
         mostCompatibleSpecie->population.push_back({0, g});
      }
   }

   //remove extinct
   species.erase(std::remove_if(species.begin(), species.end(), [](auto x){return x.population.empty();}), species.end());
}

void Population::nextGeneration(InnovationHistory& history)
{
   std::vector<Genom> newGenoms;

   for(auto& s : mSpecies)
   {
      s.selectRepresentor();
   }

   unsigned int averageComplexity = 0;
   for(auto& s : mSpecies)
   {
      for(auto& p : s.population)
      {
         averageComplexity += p.genotype.getComplexity();
      }
   }
   averageComplexity /= size();

   mEs->setGenerationResults(getAverageFitness(), averageComplexity);
   
   std::vector<unsigned int> quotas = getSpeciesOffspringQuotas();

   newGenoms.reserve(mOptimalSize);

   int specieNum = 0;
   for(auto& s : mSpecies)
   {
      s.produceOffsprings(quotas[specieNum], history, mEs->isCrossoverAllowed(), mEs->getAllowedMutations(), newGenoms);
      specieNum++;
   }

   if(mEs->isCrossoverAllowed())
   {
      for(int i = 0; i < mOptimalSize * minterspecieCrossoverPercentage / 100 || newGenoms.size() < mOptimalSize; ++i)
      {
         auto& s1 = mSpecies[Rng::genChoise(mSpecies.size())];
         auto& s2 = mSpecies[Rng::genChoise(mSpecies.size())];

         auto& p1 = s1.randomPop();
         auto& p2 = s2.randomPop();

         newGenoms.push_back(Genom::crossover(p1.genotype, p2.genotype, p1.fitness, p2.fitness));
      }
   }
   

   Speciation::respeciate(mSpecies, newGenoms, mCompatibilityFactor);
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

double Population::getAverageFitness() const
{
   Fitness totalFitness = std::accumulate(mSpecies.begin(), mSpecies.end(), 0, [](auto t, auto& s)
   {
      return t + s.getTotalFitness();
   });

   int totalPopulation = std::accumulate(mSpecies.begin(), mSpecies.end(), 0, [](auto t, auto& s)
   {
      return t + s.population.size();
   });

   return (double)totalFitness / totalPopulation;
}

Population::ConstIterator Population::begin() const
{
   return mSpecies.begin();
}

Population::ConstIterator Population::end() const
{
   return mSpecies.end();
}

Population::Iterator Population::begin()
{
   return mSpecies.begin();
}

Population::Iterator Population::end()
{
   return mSpecies.end();
}

void Population::saveState(std::ofstream& s)
{
   auto size = mSpecies.size();

   //Keeping for compatibility
   const double p1 = 0.0;
   s.write((char*)&p1, sizeof(double));
   const int p2 = 0;
   s.write((char*)&p2, sizeof(int));
   //End

   s.write((char*)&size, sizeof(std::size_t));
   for(auto& x : mSpecies)
   {
      s.write((char*)&x.id, sizeof(unsigned int));
      s.write((char*)&x.maxFitness, sizeof(double));

      //Keeping for compatibility
      const unsigned int zero = 0;
      s.write((char*)&zero, sizeof(unsigned int));
      //End

      s.write((char*)&x.sharedFitness, sizeof(double));
      s.write((char*)&x.totalFitness, sizeof(Fitness));

      auto popSize = x.population.size();
      s.write((char*)&popSize, sizeof(std::size_t));

      for(auto& y : x.population)
      {
         s.write((char*)&y.fitness, sizeof(Fitness));
         
         auto genSize = y.genotype.length();
         s.write((char*)&genSize, sizeof(std::size_t));

         for(auto& z : y.genotype)
         {
            s.write((char*)&z.innovationNumber, sizeof(InnovationNumber));
            s.write((char*)&z.weight, sizeof(double));
            s.write((char*)&z.enabled, sizeof(bool));
         }
      }
   }
}

void Population::loadState(
   std::ifstream& s, 
   InnovationHistory& history, 
   const NodeId numInputs, 
   const NodeId numOutputs
   )
{
   mSpecies.clear();

   //Keeping for compatibility
   const double p1 = 0.0;
   s.read((char*)&p1, sizeof(double));
   const int p2 = 0;
   s.read((char*)&p2, sizeof(int));
   //End

   std::size_t numSpecies = 0;
   s.read((char*)&numSpecies, sizeof(std::size_t));

   mSpecies.reserve(numSpecies);
   for(std::size_t i = 0; i < numSpecies; ++i)
   {
      Specie x;

      std::cout << ".";
      s.read((char*)&x.id, sizeof(unsigned int));
      s.read((char*)&x.maxFitness, sizeof(double));

      //Keeping for compatibility
      const unsigned int unused = 0;
      s.read((char*)&unused, sizeof(unsigned int));
      //End

      s.read((char*)&x.sharedFitness, sizeof(double));
      s.read((char*)&x.totalFitness, sizeof(Fitness));

      std::size_t popSize = 0;
      s.read((char*)&popSize, sizeof(std::size_t));

      x.population.reserve(popSize);

      for(std::size_t j = 0; j < popSize; ++j)
      {
         Pop y{0, Genom(numInputs, numOutputs)};

         s.read((char*)&y.fitness, sizeof(Fitness));

         std::size_t genomSize = 0;
         s.read((char*)&genomSize, sizeof(std::size_t));

         for(std::size_t k = 0; k < genomSize; ++k)
         {
            Gene z;

            s.read((char*)&z.innovationNumber, sizeof(InnovationNumber));
            s.read((char*)&z.weight, sizeof(double));
            s.read((char*)&z.enabled, sizeof(bool));

            auto innovation = history.get(z.innovationNumber);

            z.srcNodeId = innovation.first;
            z.dstNodeId = innovation.second;

            y.genotype += z;
         }

         x.population.push_back(y);
      }

      mSpecies.push_back(x);
   }
}

void Population::setEvolutionStrategy(std::shared_ptr<IEvolutionStrategy>& es)
{
   mEs = es;
}

}