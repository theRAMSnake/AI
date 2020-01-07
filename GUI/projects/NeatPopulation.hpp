#pragma once

#include "../IPopulation.hpp"

class NeatPopulation : public IPopulation
{
public:
   neuroevolution::Fitness getAverageFitness() const override;
   std::size_t size() const override;
   unsigned int getNumSpecies() const override;
};