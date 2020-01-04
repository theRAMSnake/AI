#include "EmptyPG.hpp"

class EmptyFitnessEvaluator : public neat::IFitnessEvaluator
{
public:
   EmptyFitnessEvaluator()
   {
      
   }

   void step()
   {
      
   }

   neat::Fitness evaluate(const neat::v2::Genom& g) override
   {
      return 0;
   }
};

neat::IFitnessEvaluator& EmptyPG::getFitnessEvaluator()
{
   return *mFitnessEvaluator;
}

EmptyPG::EmptyPG()
: mFitnessEvaluator(new EmptyFitnessEvaluator)
{
   
}

void EmptyPG::step()
{
   
}

void EmptyPG::play(const neat::v2::Genom& g)
{
   
}

std::string EmptyPG::getName() const
{
   return "Empty";
}

unsigned int EmptyPG::getNumInputs() const
{
   return 2;
}

unsigned int EmptyPG::getNumOutputs() const
{
   return 2;
}
