#include "pop.hpp"
#include "gacommon/IPlayground.hpp"
#include "gacommon/rng.hpp"
#include <boost/property_tree/ptree_fwd.hpp>
#include <iostream>
#include <numeric>
#include <variant>
#include <vector>
#include "mutations.hpp"

namespace snake4
{

Pop::Pop()
{
}

Pop& Pop::operator = (const Pop& other)
{
   fitness = other.fitness;
   blocks = other.blocks;

   return *this;
}

Pop Pop::cloneMutated(const gacommon::IODefinition& io) const
{
   Pop result;

   result.fitness = 0;
   result.blocks = mutate(io, blocks);

   return result;
}

Pop Pop::createMinimal(const gacommon::IODefinition& io)
{
    Pop result;

    auto manipulator = generateRandomManipulator(io);
    auto activator = generateRandomSensor(io);

    result.blocks.push_back({activator, manipulator});

    return result;
}

std::unique_ptr<gacommon::IAgent> Pop::createAgent(const gacommon::IODefinition& io) const
{
    return createAgentImpl(io, blocks);
}

std::string toBitString(const std::vector<std::uint8_t>& in)
{
    std::string result;
    result.resize(in.size());

    std::transform(in.begin(), in.end(), result.begin(), [](auto x){return x == 0 ? '0' : '1';});

    return result;
}
std::vector<std::uint8_t> fromBitString(const std::string& in)
{
    std::vector<std::uint8_t> result;
    result.resize(in.size());

    std::transform(in.begin(), in.end(), result.begin(), [](auto x){return x == '0' ? 0 : 1;});

    return result;
}
void serialize(const AlwaysActivator& act, boost::property_tree::ptree& ar)
{
    ar.put("type", "AlwaysActivator");
}
void serialize(const SensoricActivatorForValue& act, boost::property_tree::ptree& ar)
{
    ar.put("type", "SensoricActivatorForValue");
    ar.put("a", act.a);
    ar.put("b", act.b);
    ar.put("inputIdx", act.inputIdx);
    ar.put("ftype", static_cast<int>(act.type));
}
void serialize(const SensoricActivatorForChoice& act, boost::property_tree::ptree& ar)
{
    ar.put("type", "SensoricActivatorForChoice");
    ar.put("inversion", act.inversion);
    ar.put("inputIdx", act.inputIdx);
    ar.put("values", toBitString(act.values));
}
void serialize(const SetValueManipulator& act, boost::property_tree::ptree& ar)
{
    ar.put("type", "SetValueManipulator");
    ar.put("funcType", static_cast<int>(act.funcType));
    ar.put("outputIdx", act.outputIdx);
}
void serialize(const ChoiceVoteManipulator& act, boost::property_tree::ptree& ar)
{
    ar.put("type", "ChoiceVoteManipulator");
    ar.put("selection", act.selection);
    ar.put("outputIdx", act.outputIdx);
}
SensoricActivatorForValue deserialize(const SensoricActivatorForValue& act, const boost::property_tree::ptree& ar)
{
    SensoricActivatorForValue result;
    result.a = ar.get<double>("a");
    result.b = ar.get<double>("b");
    result.inputIdx = ar.get<std::size_t>("inputIdx");
    result.type = static_cast<SensoricActivatorForValue::Type>(ar.get<int>("ftype"));

    return result;
}
SensoricActivatorForChoice deserialize(const SensoricActivatorForChoice& act, const boost::property_tree::ptree& ar)
{
    SensoricActivatorForChoice result;
    result.inversion = ar.get<bool>("inversion");
    result.inputIdx = ar.get<std::size_t>("inputIdx");
    result.values = fromBitString(ar.get<std::string>("values"));

    return result;
}
SetValueManipulator deserialize(const SetValueManipulator& act, const boost::property_tree::ptree& ar)
{
    SetValueManipulator result;
    result.outputIdx = ar.get<std::size_t>("outputIdx");
    result.funcType = static_cast<SetValueManipulator::FunctionType>(ar.get<int>("funcType"));

    return result;
}
ChoiceVoteManipulator deserialize(const ChoiceVoteManipulator& act, const boost::property_tree::ptree& ar)
{
    ChoiceVoteManipulator result;
    result.outputIdx = ar.get<std::size_t>("outputIdx");
    result.selection = ar.get<std::size_t>("selection");

    return result;
}

void Pop::saveState(boost::property_tree::ptree& ar) const
{
   ar.put("fitness", fitness);
   boost::property_tree::ptree blocksAr;
   for(const auto& p : blocks)
   {
       boost::property_tree::ptree block;

       boost::property_tree::ptree activator;
       std::visit([&activator](const auto& arg){
               serialize(arg, activator);
           }, p.activator);

       boost::property_tree::ptree force;
       std::visit([&force](const auto& arg){
               serialize(arg, force);
           }, p.force);

       block.put_child("activator", activator);
       block.put_child("force", force);
       blocksAr.push_back(std::make_pair("", block));
   }

   ar.add_child("blocks", blocksAr);
}

Pop Pop::loadState(const boost::property_tree::ptree& ar)
{
    Pop result;

    result.fitness = ar.get<gacommon::Fitness>("fitness");
    for(const auto& popState : ar.get_child("blocks"))
    {
        BlockDefinition newBlock;
        const auto& activator = popState.second.get_child("activator");
        auto type = activator.get<std::string>("type");
        if(type == "AlwaysActivator")
        {
            newBlock.activator = AlwaysActivator();
        }
        else if(type == "SensoricActivatorForValue")
        {
            newBlock.activator = deserialize(SensoricActivatorForValue(), activator);
        }
        else if(type == "SensoricActivatorForChoice")
        {
            newBlock.activator = deserialize(SensoricActivatorForChoice(), activator);
        }
        else
        {
            throw std::runtime_error("Failed to deserialize type: " + type);
        }

        const auto& force = popState.second.get_child("force");
        type = force.get<std::string>("type");
        if(type == "SetValueManipulator")
        {
            newBlock.force = deserialize(SetValueManipulator(),force);
        }
        else if(type == "ChoiceVoteManipulator")
        {
            newBlock.force = deserialize(ChoiceVoteManipulator(), force);
        }
        else
        {
            throw std::runtime_error("Failed to deserialize type: " + type);
        }
        result.blocks.push_back(newBlock);
    }

    return result;
}

}
