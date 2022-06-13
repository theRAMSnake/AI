#include "system.hpp"
#include "gacommon/IPlayground.hpp"
#include <algorithm>
#include <memory>
#include <map>

namespace snake4
{

class Agent : public gacommon::IAgent
{
public:
   Agent(const std::vector<BlockDefinition>& blocks)
       : mBlocks(blocks)
   {
   }

   void reset() override
   {

   }

   void run(const std::vector<gacommon::IOElement>& inputs, std::vector<gacommon::IOElement>& outputs) override
   {
       mChoiceVotes.clear();

       for(std::size_t i = 0; i < outputs.size(); ++i)
       {
           if(std::holds_alternative<gacommon::ChoiceIO>(outputs[i]))
           {
               mChoiceVotes[i].resize(std::get<gacommon::ChoiceIO>(outputs[i]).options);
           }
       }

       for(const auto& b : mBlocks)
       {
           if(std::visit([&inputs, this](auto && arg){return processActivator(inputs, arg);}, b.activator))
           {
               std::visit([&outputs, this](auto && arg){return processForce(outputs, arg);}, b.force);
           }
       }

       for(std::size_t i = 0; i < outputs.size(); ++i)
       {
           if(std::holds_alternative<gacommon::ChoiceIO>(outputs[i]))
           {
               auto votes = mChoiceVotes[i];
               auto mostVoted = std::distance(votes.begin(), std::max_element(votes.begin(), votes.end()));
               std::get<gacommon::ChoiceIO>(outputs[i]).selection = mostVoted;
           }
       }
   }

   void toBinaryStream(std::ofstream& stream) const override
   {
       throw std::runtime_error("Not implemented");
   }

private:

   bool processActivator(const std::vector<gacommon::IOElement>& inputs, const AlwaysActivator& act)
   {
       return true;
   }

   bool processActivator(const std::vector<gacommon::IOElement>& inputs, const SensoricActivatorForValue& act)
   {
       auto& io = inputs[act.inputIdx];
       auto value = std::get<gacommon::ValueIO>(io).value;

       switch(act.type)
       {
           case SensoricActivatorForValue::Type::Above:
               return value > act.a;

           case SensoricActivatorForValue::Type::Below:
               return value < act.a;

           case SensoricActivatorForValue::Type::InRange:
               return value <= act.b && value >= act.a;

           case SensoricActivatorForValue::Type::NotInRange:
               return value >= act.b || value <= act.a;

           default:
               throw std::runtime_error("Unimplemented sensoric activator for value type: " + std::to_string(static_cast<int>(act.type)));
       }
   }

   bool processActivator(const std::vector<gacommon::IOElement>& inputs, const SensoricActivatorForChoice& act)
   {
       auto& io = inputs[act.inputIdx];
       auto value = std::get<gacommon::ChoiceIO>(io).selection;
       return act.values[value] == (act.inversion ? 0 : 1);
   }

   void processForce(std::vector<gacommon::IOElement>& outputs, const SetValueManipulator& manip)
   {
       auto& destination = std::get<gacommon::ValueIO>(outputs[manip.outputIdx]);
       if(manip.funcType == SetValueManipulator::FunctionType::One)
       {
           destination.value = 1;
       }
       else if(manip.funcType == SetValueManipulator::FunctionType::Zero)
       {
           destination.value = 0;
       }
       else if(manip.funcType == SetValueManipulator::FunctionType::Inc)
       {
           destination.value++;
       }
       else if(manip.funcType == SetValueManipulator::FunctionType::Dec)
       {
           destination.value--;
       }
       else
       {
           throw std::runtime_error("Unimplemented set value manipulator function type: " + std::to_string(static_cast<int>(manip.funcType)));
       }
   }

   void processForce(std::vector<gacommon::IOElement>& outputs, const ChoiceVoteManipulator& manip)
   {
       mChoiceVotes[manip.outputIdx][manip.selection]++;
   }

   const std::vector<BlockDefinition>& mBlocks;
   std::map<std::size_t, std::vector<std::size_t>> mChoiceVotes;
};

std::unique_ptr<gacommon::IAgent> createAgentImpl(const gacommon::IODefinition& io, const std::vector<BlockDefinition>& blocks)
{
    return std::make_unique<Agent>(blocks);
}

}

