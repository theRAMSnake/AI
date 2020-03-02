#pragma once
#include "neuroevolution/IPlayground.hpp"
#include <memory>

class TetrisFitnessEvaluator;
class TetrisPG : public neuroevolution::IPlayground
{
public:
   TetrisPG();

   neuroevolution::IFitnessEvaluator& getFitnessEvaluator() override;

   void step() override;
   void play(neuroevolution::IAgent& a) override;
   std::string getName() const override;
   neuroevolution::DomainGeometry getDomainGeometry() const override;

   unsigned int getNumInputs() const override;
   unsigned int getNumOutputs() const override;

private:
   std::shared_ptr<TetrisFitnessEvaluator> mFitnessEvaluator;
};