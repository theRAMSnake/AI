#include "CalculatorPG.hpp"
#include <gacommon/rng.hpp>
#include <stdexcept>
#include <sstream>
#include <math.h>

namespace pgs
{

class CalculatorFitnessEvaluator : public gacommon::IFitnessEvaluator
{
public:
   CalculatorFitnessEvaluator()
       : mInputs({gacommon::ValueIO(), gacommon::ValueIO(), gacommon::ChoiceIO{4}})
       , mOutputs({gacommon::ValueIO{}})
   {
       updateChallenges();
   }

   void step()
   {
       updateChallenges();
   }

   gacommon::Fitness evaluate(gacommon::IAgent& agent) override
   {
      gacommon::Fitness result = 0;

      for(auto c : mChallenges)
      {
          auto inputs = getInputs();
          auto outputs = getOutputs();

          std::get<gacommon::ValueIO>(inputs[0]).value = static_cast<double>(c.a);
          std::get<gacommon::ValueIO>(inputs[1]).value = static_cast<double>(c.b);
          std::get<gacommon::ChoiceIO>(inputs[2]).selection = static_cast<std::size_t>(c.op);

          auto expected = runChallenge(c);

          agent.reset();
          agent.run(inputs, outputs);

          if(expected == floor(std::get<gacommon::ValueIO>(outputs[0]).value))
          {
              result++;
              if(c.op == Operation::Mult)
              {
                  result++;
              }
              if(c.op == Operation::Div)
              {
                  result += 2;
              }
          }
      }

      return result;
   }

   void play(gacommon::IAgent& a, std::ostringstream& out)
   {
       updateChallenges();
       for(auto c : mChallenges)
       {
           a.reset();

           auto inputs = getInputs();
           auto outputs = getOutputs();

           std::get<gacommon::ValueIO>(inputs[0]).value = static_cast<double>(c.a);
           std::get<gacommon::ValueIO>(inputs[1]).value = static_cast<double>(c.b);
           std::get<gacommon::ChoiceIO>(inputs[2]).selection = static_cast<std::size_t>(c.op);

           auto printChChoice = [](auto op)
           {
               switch(op)
               {
                   case Operation::Add:
                       return "Add";
                   case Operation::Sub:
                       return "Sub";
                   case Operation::Mult:
                       return "Mult";
                   case Operation::Div:
                       return "Div";
               }
               return "";
           };

           out << std::string("Challenge: ") << c.a << " " << printChChoice(c.op) << " " << c.b << std::endl;

           auto expected = runChallenge(c);

           a.run(inputs, outputs);

           if(expected == floor(std::get<gacommon::ValueIO>(outputs[0]).value))
           {
               out << "Success" << std::endl;
           }
           else
           {
               out << "Given answer is " << floor(std::get<gacommon::ValueIO>(outputs[0]).value) << std::endl;
           }
       }
   }

    std::vector<gacommon::IOElement> getInputs() const
    {
        return mInputs;
    }

    std::vector<gacommon::IOElement> getOutputs() const
    {
        return mOutputs;
    }

private:
   enum class Operation
   {
       Add,
       Sub,
       Mult,
       Div
   };

   struct Challenge
   {
       unsigned int a;
       unsigned int b;
       Operation op;
   };

   void updateChallenges()
   {
       mChallenges.clear();
       for(int i = 0; i < 25; ++i)
       {
           mChallenges.push_back({ Rng::genChoise(100), Rng::genChoise(100), Operation::Add});
       }
       for(int i = 0; i < 25; ++i)
       {
           auto a = Rng::genChoise(200);
           auto b = Rng::genChoise(200);
           mChallenges.push_back({ std::max(a, b), std::min(a, b), Operation::Sub});
       }
       for(int i = 0; i < 25; ++i)
       {
           mChallenges.push_back({ Rng::genChoise(15), Rng::genChoise(15), Operation::Mult});
       }
       for(int i = 0; i < 25; ++i)
       {
           auto a = Rng::genChoise(15);
           auto b = Rng::genChoise(15);
           if(a == 0 || b == 0)
           {
               a = 1;
               b = 1;
           }
           mChallenges.push_back({ a * b, a, Operation::Div});
       }
   }

   int runChallenge(const Challenge& c) const
   {
       switch(c.op)
       {
           case Operation::Add:
               return c.a + c.b;
           case Operation::Sub:
               return c.a - c.b;
           case Operation::Mult:
               return c.a * c.b;
           case Operation::Div:
               return c.a / c.b;
       }

       throw std::runtime_error("Unexpected at runChallenge");
   }

   std::vector<Challenge> mChallenges;
   std::vector<gacommon::IOElement> mInputs;
   std::vector<gacommon::IOElement> mOutputs;
};

gacommon::IFitnessEvaluator& CalculatorPG::getFitnessEvaluator()
{
   return *mFitnessEvaluator;
}

CalculatorPG::CalculatorPG()
: mFitnessEvaluator(new CalculatorFitnessEvaluator)
{
}

void CalculatorPG::step()
{
   mFitnessEvaluator->step();
}

void CalculatorPG::play(gacommon::IAgent& a, std::ostringstream& out)
{
   mFitnessEvaluator->play(a, out);
}

std::string CalculatorPG::getName() const
{
   return "Calculator";
}

std::vector<gacommon::IOElement> CalculatorPG::getInputs() const
{
    return mFitnessEvaluator->getInputs();
}

std::vector<gacommon::IOElement> CalculatorPG::getOutputs() const
{
    return mFitnessEvaluator->getOutputs();
}

}
