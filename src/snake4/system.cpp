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
       , mPrevActivated(false)
   {
   }

   void reset() override
   {

   }

   void run(const std::vector<gacommon::IOElement>& inputs, std::vector<gacommon::IOElement>& outputs) override
   {
       for(const auto& b : mBlocks)
       {
           if(mBlockedSteps > 0)
           {
               mPrevActivated = false;
               mBlockedSteps--;
               continue;
           }

           if(std::visit([&inputs, this](auto && arg){return processActivator(inputs, arg);}, b.activator))
           {
               mPrevActivated = true;
               std::visit([&outputs, this](auto && arg){return processForce(outputs, arg);}, b.force);
           }
           else
           {
               mPrevActivated = false;
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

   bool processActivator(const std::vector<gacommon::IOElement>& inputs, const ChainActivator& act)
   {
       return mPrevActivated;
   }

   bool processActivator(const std::vector<gacommon::IOElement>& inputs, const ConsumeActivator& act)
   {
       auto pos1 = mElements.find(act.left);
       auto pos2 = mElements.find(act.left);
       if(pos1 != mElements.end() && pos2 != mElements.end() && pos1->second > 0 && pos2->second > 0)
       {
           pos1->second--;
           pos2->second--;
           mLastConsumed1 = pos1->first;
           mLastConsumed2 = pos2->first;
           return true;
       }
       return false;
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

   void processForce(std::vector<gacommon::IOElement>& outputs, const SetChoiceManipulator& manip)
   {
       auto& destination = std::get<gacommon::ChoiceIO>(outputs[manip.outputIdx]);
       destination.selection = manip.selection;
   }

   void processForce(std::vector<gacommon::IOElement>& outputs, const CombineForce& force)
   {
       mElements[mLastConsumed1 + mLastConsumed2]++;
   }

   void processForce(std::vector<gacommon::IOElement>& outputs, const SinkForce& force)
   {
   }

   void processForce(std::vector<gacommon::IOElement>& outputs, const ProduceForce& force)
   {
       mElements[force.primitive]++;
   }

   void processForce(std::vector<gacommon::IOElement>& outputs, const DecomposeForce& force)
   {
       if(force.pos < mLastConsumed1.size())
       {
           mElements[mLastConsumed1.substr(0, force.pos)]++;
           mElements[mLastConsumed1.substr(force.pos, std::string::npos)]++;
       }
       if(force.pos < mLastConsumed2.size())
       {
           mElements[mLastConsumed2.substr(0, force.pos)]++;
           mElements[mLastConsumed2.substr(force.pos, std::string::npos)]++;
       }
   }

   void processForce(std::vector<gacommon::IOElement>& outputs, const BlockForce& force)
   {
       mBlockedSteps = force.values;
   }

   const std::vector<BlockDefinition>& mBlocks;
   bool mPrevActivated;
   std::size_t mBlockedSteps = 0;
   std::string mLastConsumed1;
   std::string mLastConsumed2;
   std::map<std::string, std::size_t> mElements;
};

std::unique_ptr<gacommon::IAgent> createAgentImpl(const gacommon::IODefinition& io, const std::vector<BlockDefinition>& blocks)
{
    return std::make_unique<Agent>(blocks);
}

}

