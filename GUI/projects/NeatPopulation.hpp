#pragma once

#include "../IPopulation.hpp"

class NeatPopulation : public IPopulation
{
public:
   neuroevolution::Fitness getAverageFitness() const;
};