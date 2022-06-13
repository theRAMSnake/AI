#pragma once
#include "gacommon/IPlayground.hpp"
#include <memory>

class TetrisFitnessEvaluator;
class TetrisPG : public gacommon::IPlayground
{
public:
   TetrisPG();

   gacommon::IFitnessEvaluator& getFitnessEvaluator() override;

   void step() override;
   void play(gacommon::IAgent& a, std::ostringstream& out) override;
   std::string getName() const override;

   std::vector<gacommon::IOElement> getInputs() const override;
   std::vector<gacommon::IOElement> getOutputs() const override;

private:
   std::shared_ptr<TetrisFitnessEvaluator> mFitnessEvaluator;
};
