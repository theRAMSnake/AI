#pragma once
#include "neat/neat.hpp"

namespace neat
{
   class IFitnessEvaluator;
}

class IPlayground
{
public:
   virtual neat::IFitnessEvaluator& getFitnessEvaluator() = 0;
   virtual std::string getName() const = 0;

   virtual void step() = 0;
   virtual void play(const neat::v2::Genom& g) = 0;

   virtual unsigned int getNumInputs() const = 0;
   virtual unsigned int getNumOutputs() const = 0;

   ~IPlayground(){}
};