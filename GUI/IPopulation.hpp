#pragma once

#include "neuroevolution/neuro_net.hpp"
#include "neuroevolution/IPlayground.hpp"

class IPopulation
{
public:
   virtual neuroevolution::Fitness getAverageFitness() const = 0;
   virtual std::size_t size() const = 0;
   virtual unsigned int getNumSpecies() const = 0;

   virtual ~IPopulation(){}
};