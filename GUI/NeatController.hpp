#pragma once
#include "neat/neat.hpp"
#include "IPlayground.hpp"

class NeatController
{
public:
   NeatController(IPlayground& pg);

   void step();
   void saveState(const std::string& filename);
   void loadState(const std::string& filename);
   void setGeneration(const unsigned int generation);

   const neat::Population& getPopulation() const;
   const unsigned int getGeneration() const;

private:
   unsigned int mGeneration = 0;
   neat::Neat mNeat;
};