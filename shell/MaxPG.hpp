#pragma once
#include "IPlayground.hpp"
#include <memory>

class FitnessEvaluator1;
class MaxPG : public IPlayground
{
public:
   MaxPG();

   neat::Config getConfig() override;
   neat::IFitnessEvaluator& getFitnessEvaluator() override;

   void step() override;
   void play(const neat::Genom& g) override;

private:
   std::shared_ptr<FitnessEvaluator1> mFitnessEvaluator;
};