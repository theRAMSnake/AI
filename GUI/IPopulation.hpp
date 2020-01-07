#pragma once

#include "neuroevolution/neuro_net.hpp"

class IPopulation
{
public:
   virtual neuroevolution::Fitness getAverageFitness() const = 0;

   virtual ~IPopulation(){}
};