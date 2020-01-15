#pragma once

#include <string>
#include <vector>

namespace neuroevolution
{

struct Point3D
{
   unsigned int x;
   unsigned int y;
   unsigned int z;
};

struct Point2D
{
   unsigned int x;
   unsigned int y;
};

struct DomainGeometry
{
   Point3D size;
   std::vector<Point2D> inputs;
   std::vector<Point2D> outputs;
};

using Fitness = int;

class NeuroNet;
class IFitnessEvaluator
{
public:
    virtual Fitness evaluate(NeuroNet& ann) = 0;

    virtual ~IFitnessEvaluator(){}
};

class IPlayground
{
public:
   virtual IFitnessEvaluator& getFitnessEvaluator() = 0;
   virtual std::string getName() const = 0;
   virtual DomainGeometry getDomainGeometry() const = 0;

   virtual void step() = 0;
   virtual void play(NeuroNet& ann) = 0;

   virtual unsigned int getNumInputs() const = 0;
   virtual unsigned int getNumOutputs() const = 0;

   ~IPlayground(){}
};

}
