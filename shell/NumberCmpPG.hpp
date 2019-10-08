#pragma once
#include "IPlayground.hpp"
#include <memory>

class FitnessEvaluator;
class NumberCmpPG : public IPlayground
{
public:
   NumberCmpPG();

   neat::Config getConfig() override;
   neat::IFitnessEvaluator& getFitnessEvaluator() override;

   void step() override;

private:
   std::shared_ptr<FitnessEvaluator> mFitnessEvaluator;
};