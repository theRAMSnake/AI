#pragma once
#include "neuroevolution/IPlayground.hpp"
#include <memory>

class LinesFitnessEvaluator;
class LinesPG : public neuroevolution::IPlayground
{
public:
   LinesPG();

   neuroevolution::IFitnessEvaluator& getFitnessEvaluator() override;

   void step() override;
   void play(neuroevolution::NeuroNet& ann) override;
   std::string getName() const override;

   unsigned int getNumInputs() const override;
   unsigned int getNumOutputs() const override;

private:
   std::shared_ptr<LinesFitnessEvaluator> mFitnessEvaluator;
};