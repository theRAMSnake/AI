#include "LogicalPG.hpp"
#include <gacommon/rng.hpp>
#include <stdexcept>
#include <sstream>

namespace pgs
{

class LogicalFitnessEvaluator : public gacommon::IFitnessEvaluator
{
public:
   LogicalFitnessEvaluator()
       : mInputs({gacommon::ValueIO(), gacommon::ValueIO(), gacommon::ChoiceIO{4}})
       , mOutputs({gacommon::ChoiceIO{2}})
   {
       mChallenges.push_back({0, 0, Operation::Or});
       mChallenges.push_back({0, 1, Operation::Or});
       mChallenges.push_back({1, 0, Operation::Or});
       mChallenges.push_back({1, 1, Operation::Or});
       mChallenges.push_back({0, 0, Operation::And});
       mChallenges.push_back({0, 1, Operation::And});
       mChallenges.push_back({1, 0, Operation::And});
       mChallenges.push_back({1, 1, Operation::And});
       mChallenges.push_back({0, 0, Operation::Nand});
       mChallenges.push_back({0, 1, Operation::Nand});
       mChallenges.push_back({1, 0, Operation::Nand});
       mChallenges.push_back({1, 1, Operation::Nand});
       mChallenges.push_back({0, 0, Operation::Xor});
       mChallenges.push_back({0, 1, Operation::Xor});
       mChallenges.push_back({1, 0, Operation::Xor});
       mChallenges.push_back({1, 1, Operation::Xor});
   }

   void step()
   {
   }
   
   gacommon::Fitness evaluate(gacommon::IAgent& agent) override
   {
      gacommon::Fitness result = 0;
      
      for(auto c : mChallenges)
      {
          agent.reset();

          auto inputs = getInputs();
          auto outputs = getOutputs();

          std::get<gacommon::ValueIO>(inputs[0]).value = static_cast<double>(c.a);
          std::get<gacommon::ValueIO>(inputs[1]).value = static_cast<double>(c.b);
          std::get<gacommon::ChoiceIO>(inputs[2]).selection = static_cast<std::size_t>(c.op);

          auto expected = runChallenge(c);

          agent.run(inputs, outputs);

          if(expected == std::get<gacommon::ChoiceIO>(outputs[0]).selection)
          {
              result++;
          } 
      }

      return result;
   }

   void play(gacommon::IAgent& a, std::ostringstream& out)
   {
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
                   case Operation::Or:
                       return "OR";
                   case Operation::And:
                       return "AND";
                   case Operation::Xor:
                       return "XOR";
                   case Operation::Nand:
                       return "NAND";
               }
               return "";
           };

           out << std::string("Challenge: ") << c.a << " " << printChChoice(c.op) << " " << c.b << std::endl;

           auto expected = runChallenge(c);

           a.run(inputs, outputs);

           if(expected == std::get<gacommon::ChoiceIO>(outputs[0]).selection)
           {
               out << "Success" << std::endl;
           }
           else
           {
               out << "Fail" << std::endl;
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
       Or,
       Xor,
       And,
       Nand
   };

   struct Challenge
   {
       int a;
       int b;
       Operation op;
   };

   std::size_t runChallenge(const Challenge& c) const
   {
       switch(c.op)
       {
           case Operation::Or:
               return (c.a || c.b) ? 1 : 0;
           case Operation::And:
               return (c.a && c.b) ? 1 : 0;
           case Operation::Xor:
               return (c.a ^ c.b) ? 1 : 0;
           case Operation::Nand:
               return !(c.a && c.b) ? 1 : 0;
       }

       throw std::runtime_error("Unexpected at runChallenge");
   }

   std::vector<Challenge> mChallenges;
   std::vector<gacommon::IOElement> mInputs;
   std::vector<gacommon::IOElement> mOutputs;
};

gacommon::IFitnessEvaluator& LogicalPG::getFitnessEvaluator()
{
   return *mFitnessEvaluator;
}

LogicalPG::LogicalPG()
: mFitnessEvaluator(new LogicalFitnessEvaluator)
{
   
}

void LogicalPG::step()
{
   mFitnessEvaluator->step();
}

void LogicalPG::play(gacommon::IAgent& a, std::ostringstream& out)
{
   mFitnessEvaluator->play(a, out);
}

std::string LogicalPG::getName() const
{
   return "Logic";
}

std::vector<gacommon::IOElement> LogicalPG::getInputs() const
{
    return mFitnessEvaluator->getInputs();
}

std::vector<gacommon::IOElement> LogicalPG::getOutputs() const
{
    return mFitnessEvaluator->getOutputs();
}

}
