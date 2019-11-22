#pragma once
#include "IPlayground.hpp"
#include <memory>

class CPFitnessEvaluator;
class CheckpointPG : public IPlayground
{
public:
   CheckpointPG();

   neat::Config getConfig() override;
   neat::IFitnessEvaluator& getFitnessEvaluator() override;

   void step() override;
   void play(const neat::Genom& g) override;
   std::string getName() const override;

private:
   std::shared_ptr<CPFitnessEvaluator> mFitnessEvaluator;
};