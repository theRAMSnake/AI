#pragma once

#include "../IPopulation.hpp"
#include "neat/neat.hpp"

class NeatPopulation : public IPopulation
{
public:
   NeatPopulation(const neat::Neat& src);

   double getAverageFitness() const override;
   double getTopFitness() const override;
   double getAverageComplexity() const override;
   std::size_t size() const override;
   unsigned int getNumSpecies() const override;
   std::vector<SpecieResults> getSpecies() const override;
   std::unique_ptr<gacommon::IAgent> createAgent(const PopResult& pop) const override;

private:
   const neat::Neat& mSrc;
};
