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

class IAgent
{
public:
   virtual void reset() = 0;
   virtual unsigned int run(const double* input) = 0;
   virtual void toBinaryStream(std::ofstream& stream) const = 0;
   virtual ~IAgent(){}
};

class IFitnessEvaluator
{
public:
    virtual Fitness evaluate(IAgent& agent) = 0;

    virtual ~IFitnessEvaluator(){}
};

class IPlayground
{
public:
   virtual IFitnessEvaluator& getFitnessEvaluator() = 0;
   virtual std::string getName() const = 0;
   virtual DomainGeometry getDomainGeometry() const = 0;

   virtual void step() = 0;
   virtual void play(IAgent& agent) = 0;

   virtual unsigned int getNumInputs() const = 0;
   virtual unsigned int getNumOutputs() const = 0;

   ~IPlayground(){}
};

}
