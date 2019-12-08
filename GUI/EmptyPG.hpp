#pragma once
#include "IPlayground.hpp"
#include <memory>

class EmptyFitnessEvaluator;
class EmptyPG : public IPlayground
{
public:
   EmptyPG();

   neat::IFitnessEvaluator& getFitnessEvaluator() override;

   void step() override;
   void play(const neat::Genom& g) override;
   std::string getName() const override;

   unsigned int getNumInputs() const override;
   unsigned int getNumOutputs() const override;

private:
   std::shared_ptr<EmptyFitnessEvaluator> mFitnessEvaluator;
};