#pragma once

namespace neuroevolution
{

using Fitness = int;

class IFitnessEvaluator
{
public:
    virtual Fitness evaluate(const neuronet::NeuroNet& ann) = 0;

    virtual ~IFitnessEvaluator(){}
};

class IPlayground
{
public:
   virtual IFitnessEvaluator& getFitnessEvaluator() = 0;
   virtual std::string getName() const = 0;

   virtual void step() = 0;
   virtual void play(const neuronet::NeuroNet& ann) = 0;

   virtual unsigned int getNumInputs() const = 0;
   virtual unsigned int getNumOutputs() const = 0;

   ~IPlayground(){}
};

}
