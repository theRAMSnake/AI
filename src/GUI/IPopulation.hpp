#pragma once

#include "gacommon/neuro_net2.hpp"
#include "gacommon/IPlayground.hpp"
#include <memory>

struct PopResult
{
   unsigned int specieId;
   unsigned int organismId;

   gacommon::Fitness fitness;
   std::size_t complexity;
   std::size_t nodeCount;
};

struct SpecieResults
{
   unsigned int id = 0;

   double averageFitness = 0.0;
   gacommon::Fitness maxFitness;
   std::vector<PopResult> popResults;
};

class IPopulation
{
public:  
   virtual double getAverageFitness() const = 0;
   virtual double getTopFitness() const = 0;
   virtual double getAverageComplexity() const = 0;
   virtual std::size_t size() const = 0;
   virtual unsigned int getNumSpecies() const = 0;

   virtual std::vector<SpecieResults> getSpecies() const = 0;
   virtual std::unique_ptr<gacommon::IAgent> createAgent(const PopResult& pop) const = 0;

   virtual ~IPopulation(){}
};
