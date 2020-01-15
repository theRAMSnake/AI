#pragma once
#include "neuroevolution/IPlayground.hpp"
#include <memory>

class CPFitnessEvaluator;
class CheckpointPG : public neuroevolution::IPlayground
{
public:
   CheckpointPG();

   neuroevolution::IFitnessEvaluator& getFitnessEvaluator() override;

   void step() override;
   void play(neuroevolution::NeuroNet& ann) override;
   std::string getName() const override;
   neuroevolution::DomainGeometry getDomainGeometry() const override;

   unsigned int getNumInputs() const override;
   unsigned int getNumOutputs() const override;

private:
   std::shared_ptr<CPFitnessEvaluator> mFitnessEvaluator;
};