#pragma once
#include "gacommon/IPlayground.hpp"
#include <memory>

namespace pgs
{

//Playground to play with calculator questions.
//Inputs: value1, value2, choice(ADD, SUB, MULT, DIV)
//Outputs: value1 - result
//Note: DIV by 0 is never proposed
//Note: Only positive integers
//Points awarded as follows:
// ADD or SUB - 1 points
// MULT - 2 points
// DIV - 3 points
// 25 random questions asked per each category, for a total maximum fitness of 175
class CalculatorFitnessEvaluator;
class CalculatorPG : public gacommon::IPlayground
{
public:
   CalculatorPG();

   gacommon::IFitnessEvaluator& getFitnessEvaluator() override;

   void step() override;
   void play(gacommon::IAgent& a, std::ostringstream& out) override;
   std::string getName() const override;

   std::vector<gacommon::IOElement> getInputs() const override;
   std::vector<gacommon::IOElement> getOutputs() const override;

private:
   std::shared_ptr<CalculatorFitnessEvaluator> mFitnessEvaluator;
};

}
