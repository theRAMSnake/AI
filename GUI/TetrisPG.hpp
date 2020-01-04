#pragma once
#include "IPlayground.hpp"
#include <memory>

class TetrisFitnessEvaluator;
class TetrisPG : public IPlayground
{
public:
   TetrisPG();

   neat::IFitnessEvaluator& getFitnessEvaluator() override;

   void step() override;
   void play(const neat::v2::Genom& g) override;
   std::string getName() const override;

   unsigned int getNumInputs() const override;
   unsigned int getNumOutputs() const override;

private:
   std::shared_ptr<TetrisFitnessEvaluator> mFitnessEvaluator;
};