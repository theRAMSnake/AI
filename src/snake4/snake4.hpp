#pragma once
#include "gacommon/IPlayground.hpp"
#include "gacommon/natural_selection.hpp"
#include "pop.hpp"
#include <memory>

namespace snake4
{

class Algorithm
{
public:
   Algorithm(
      const gacommon::Config& cfg,
      const gacommon::IODefinition& io,
      gacommon::IFitnessEvaluator& fitnessEvaluator
      );

   void step();
   void reconfigure(const gacommon::Config& cfg);

   const std::vector<Pop>& getPopulation() const;
   std::size_t getGenerationNumber() const;

   void saveState(const std::string& fileName);
   void loadState(const std::string& fileName);

private:
   gacommon::NaturalSelection<Pop> mImpl;
};

}
