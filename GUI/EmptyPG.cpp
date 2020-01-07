#include "EmptyPG.hpp"

class EmptyFitnessEvaluator : public neuroevolution::IFitnessEvaluator
{
public:
   EmptyFitnessEvaluator()
   {
      
   }

   void step()
   {
      
   }

   neuroevolution::Fitness evaluate(neuroevolution::NeuroNet& ann) override
   {
      return 0;
   }
};

neuroevolution::IFitnessEvaluator& EmptyPG::getFitnessEvaluator()
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

void EmptyPG::play(neuroevolution::NeuroNet& ann)
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
