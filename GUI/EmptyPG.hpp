#pragma once
#include "neuroevolution/IPlayground.hpp"
#include <memory>

class EmptyFitnessEvaluator;
class EmptyPG : public neuroevolution::IPlayground
{
public:
   EmptyPG();

   neuroevolution::IFitnessEvaluator& getFitnessEvaluator() override;

   void step() override;
   void play(neuroevolution::NeuroNet& ann) override;
   std::string getName() const override;

   unsigned int getNumInputs() const override;
   unsigned int getNumOutputs() const override;

private:
   std::shared_ptr<EmptyFitnessEvaluator> mFitnessEvaluator;
};