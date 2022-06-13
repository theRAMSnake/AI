#pragma once
#include "gacommon/IPlayground.hpp"
#include <memory>

namespace pgs
{

//Playground to play with logical questions
//Inputs: value1, value2, choice(OR, AND, XOR, NAND)
//Outputs: choice(0, 1)
class LogicalFitnessEvaluator;
class LogicalPG : public gacommon::IPlayground
{
public:
   LogicalPG();

   gacommon::IFitnessEvaluator& getFitnessEvaluator() override;

   void step() override;
   void play(gacommon::IAgent& a, std::ostringstream& out) override;
   std::string getName() const override;

   std::vector<gacommon::IOElement> getInputs() const override;
   std::vector<gacommon::IOElement> getOutputs() const override;

private:
   std::shared_ptr<LogicalFitnessEvaluator> mFitnessEvaluator;
};

}
