#include "population.hpp"
#include "rng.hpp"
#include <algorithm>
#include <numeric>
#include <iostream>
#include <string>
#include "../logger/Logger.hpp"

namespace neat
{

bool comparePopsByFitness(const Pop& a, const Pop& b)
{
   return a.fitness > b.fitness;
}

Population Population::createInitialPopulation(
   const NodeId numInputs, 
   const NodeId numOutputs, 
   const Config& config,
   InnovationHistory& history
   )
{
   Population p(config);

   Specie s;
   s.id = 0;
   s.maxFitness = 0;

   for(unsigned int i = 0; i < config.size; ++i)
   {
      s.population.push_back({0, v2::Genom::createMinimal(numInputs, numOutputs, history, true)});
   }

   p.mSpecies.push_back(s);

   return p;
}

double Specie::getSharedFitness() const
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

   if (sharedFitness > maxAverageFitness)
   {
       maxAverageFitness = sharedFitness;
       numStagnant = 0;
   }
   else
   {
       numStagnant++;
   }

   maxFitness = population[0].fitness;
}

void Specie::selectRepresentor()
{
   representor = randomPop();
}

Population::Population(const Config& config)
: mCfg(config)
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
         numOffspringsPerSpecie.push_back(mCfg.size / mSpecies.size());
      }
   }
   else if (mEs->enableDoomsday() && mNumStagnantGenerations > 100)
   {
       for (std::size_t i = 0; i < mSpecies.size(); ++i)
       {
           numOffspringsPerSpecie.push_back(0);
       }

       auto bestSpecie = std::max_element(mSpecies.begin(), mSpecies.end(), [](auto & a, auto & b) {return a.maxFitness < b.maxFitness; });
       numOffspringsPerSpecie[std::distance(mSpecies.begin(), bestSpecie)] = mCfg.size / 2;

       auto bestSpecieFitness = bestSpecie->maxFitness;
       bestSpecie->maxFitness = 0.0;

       auto secondBestSpecie = std::max_element(mSpecies.begin(), mSpecies.end(), [](auto & a, auto & b) {return a.maxFitness < b.maxFitness; });
       numOffspringsPerSpecie[std::distance(mSpecies.begin(), secondBestSpecie)] = mCfg.size / 2;

       bestSpecie->maxFitness = bestSpecieFitness;

       mNumStagnantGenerations = 0;
       mMaxComplexity = 0.0;
   }
   else
   {
      for(std::size_t i = 0; i < mSpecies.size(); ++i)
      {
          if (mSpecies[i].isStagnant())
          {
              numOffspringsPerSpecie.push_back(0);
          }
          else
          {
              numOffspringsPerSpecie.push_back(mSpecies[i].getTotalFitness() / double(getAverageFitness() + 0.001));
          }
      }
   }

   return numOffspringsPerSpecie;
}



Fitness Specie::getTotalFitness() const
{
   return totalFitness;
}

bool Specie::isStagnant() const
{
    return false;
}

void Specie::produceOffsprings(
   const unsigned int amount, 
   InnovationHistory& history, 
   const bool isCrossoverAllowed,
   const v2::MutationConfig mutationCfg,
   std::vector<v2::Genom>& out
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

   auto quorterSize = population.size() / 2;
   while (population.size() > 1 && population.size() > quorterSize)//Keep at least one organisms
   {
       population.pop_back();
   }

   for(unsigned int i = 0; i < amountLeft; ++i)
   {
      auto& pop1 = randomPop();

      if(!isCrossoverAllowed || Rng::genProbability(0.5) || population.size() == 1)
      {
            auto genom = pop1.genotype;
            genom.mutate(mutationCfg, history);
            out.push_back(genom);
      }
      else
      {
            auto* pop2 = &pop1;
            while(pop2 == &pop1)
            {
               pop2 = &randomPop();
            }
            
            auto genom = v2::Genom::crossover(pop1.genotype, pop2->genotype, pop1.fitness, pop2->fitness);
            genom.mutate(mutationCfg, history);
            out.push_back(genom);
      }
   }
}

unsigned int Speciation::genNewSpecieId(const std::vector<Specie>& species)
{
   return std::max_element(species.begin(), species.end(), [] (auto x, auto y){return x.id < y.id;})->id + 1;
}

void Population::reconfigure(const Config& config)
{
   mCfg = config;
}

void Speciation::respeciate(
   std::vector<Specie>& species, 
   const std::vector<v2::Genom>& genoms, 
   const double compatibilityFactor,
   const double C1_C2,
   const double C3
   )
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
         auto distance = v2::Genom::calculateDivergence(g, s.representor->genotype, C1_C2, C3);
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
   std::vector<v2::Genom> newGenoms;

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
   mAverageComplexity = static_cast<double>(averageComplexity) / size();

   mEs->setGenerationResults(getAverageFitness(), mAverageComplexity);
   
   std::vector<unsigned int> quotas = getSpeciesOffspringQuotas();

   newGenoms.reserve(mCfg.size);

   auto bestSpecie = std::max_element(mSpecies.begin(), mSpecies.end(), [](auto & a, auto & b) {return a.maxFitness < b.maxFitness; });
   newGenoms.push_back(bestSpecie->population[0].genotype);

   int specieNum = 0;
   for(auto& s : mSpecies)
   {
      s.produceOffsprings(quotas[specieNum], history, mEs->isCrossoverAllowed(), mEs->getAllowedMutations(), newGenoms);
      specieNum++;
   }

   if(mEs->isCrossoverAllowed())
   {
      for(int i = 0; i < mCfg.size * mCfg.minterspecieCrossoverPercentage / 100 && newGenoms.size() < mCfg.size; ++i)
      {
         auto& s1 = mSpecies[Rng::genChoise(mSpecies.size())];
         auto& s2 = mSpecies[Rng::genChoise(mSpecies.size())];

         auto& p1 = s1.randomPop();
         auto& p2 = s2.randomPop();

         newGenoms.push_back(v2::Genom::crossover(p1.genotype, p2.genotype, p1.fitness, p2.fitness));
      }
   }
   
   //Fill population up to level
   while (newGenoms.size() < mCfg.size)
   {
       auto& s1 = mSpecies[Rng::genChoise(mSpecies.size())];
       auto& p1 = s1.randomPop();

       newGenoms.push_back(p1.genotype);
   }

   Speciation::respeciate(mSpecies, newGenoms, mCfg.mCompatibilityFactor, mCfg.mC1_C2, mCfg.mC3);
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

   s.write((char*)&size, sizeof(std::size_t));
   for(auto& x : mSpecies)
   {
      s.write((char*)&x.id, sizeof(unsigned int));
      s.write((char*)&x.maxFitness, sizeof(double));

      s.write((char*)&x.sharedFitness, sizeof(double));
      s.write((char*)&x.totalFitness, sizeof(Fitness));

      auto popSize = x.population.size();
      s.write((char*)&popSize, sizeof(std::size_t));

      for(auto& y : x.population)
      {
         s.write((char*)&y.fitness, sizeof(Fitness));

         y.genotype.write(s);
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

   std::size_t numSpecies = 0;
   s.read((char*)&numSpecies, sizeof(std::size_t));

   mSpecies.reserve(numSpecies);
   for(std::size_t i = 0; i < numSpecies; ++i)
   {
      Specie x;

      std::cout << ".";
      s.read((char*)&x.id, sizeof(unsigned int));
      s.read((char*)&x.maxFitness, sizeof(double));

      s.read((char*)&x.sharedFitness, sizeof(double));
      s.read((char*)&x.totalFitness, sizeof(Fitness));

      std::size_t popSize = 0;
      s.read((char*)&popSize, sizeof(std::size_t));

      x.population.reserve(popSize);

      for(std::size_t j = 0; j < popSize; ++j)
      {
         Pop y{0, v2::Genom(numInputs, numOutputs)};

         s.read((char*)&y.fitness, sizeof(Fitness));

         y.genotype = v2::Genom::read(s, numInputs, numOutputs, history);

         x.population.push_back(y);
      }

      mSpecies.push_back(x);
   }
}

void Population::setEvolutionStrategy(std::shared_ptr<IEvolutionStrategy>& es)
{
   mEs = es;

   std::cout << mEs->getAllowedMutations().addNodeMutationChance << " " << mEs->getAllowedMutations().addConnectionMutationChance;
}

double Population::getAverageComplexity() const
{
   return mAverageComplexity;
}

}