#pragma once
#include "neat/neat.hpp"

namespace neat
{
   class IFitnessEvaluator;
}

class IPlayground
{
public:
   virtual neat::Config getConfig() = 0;
   virtual neat::IFitnessEvaluator& getFitnessEvaluator() = 0;
   virtual std::string getName() const = 0;

   virtual void step() = 0;
   virtual void play(const neat::Genom& g) = 0;

   ~IPlayground(){}
};