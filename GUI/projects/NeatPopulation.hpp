#pragma once

#include "../IPopulation.hpp"

class NeatPopulation : public IPopulation
{
public:
   neuroevolution::Fitness getAverageFitness() const override;
   double getAverageComplexity() const override;
   std::size_t size() const override;
   unsigned int getNumSpecies() const override;
   std::vector<SpecieResults> getSpecies() const override;
   std::unique_ptr<neuroevolution::NeuroNet> createAnn(const PopResult& pop) const override;
};