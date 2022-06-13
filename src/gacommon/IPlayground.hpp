#pragma once

#include <string>
#include <vector>
#include <variant>

namespace gacommon
{

using Fitness = int;

//Rename & namespace
enum class IOType
{
    Value,

    Choice,

    Bitmap
};

//Single double value
struct ValueIO
{
    double value;
};

//Choice, i.e. one and only one of the options must be selected
struct ChoiceIO
{
    const std::size_t options;
    std::size_t selection;
};

//2D bit map
struct BitmapIO
{
    const std::size_t width;
    const std::size_t heigth;
    std::vector<std::uint8_t> map;
};

using IOElement = std::variant<ValueIO, ChoiceIO, BitmapIO>;

class IAgent
{
public:
   virtual void reset() = 0;
   virtual void run(const std::vector<IOElement>& inputs, std::vector<IOElement>& output) = 0;
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

   virtual void step() = 0;
   virtual void play(IAgent& agent, std::ostringstream& output) = 0;

   virtual std::vector<IOElement> getInputs() const = 0;
   virtual std::vector<IOElement> getOutputs() const = 0;

   ~IPlayground(){}
};

}
