#include "mutations.hpp"
#include "gacommon/IPlayground.hpp"
#include "gacommon/rng.hpp"
#include <stdexcept>
#include <variant>

namespace snake4
{

std::string generatePrimitive()
{
    static const std::vector<std::string> options = {
        "A", "B", "C", "D", "E", "F"
    };

    return options[Rng::genChoise(6)];
}

ActivatorDefinition generateRandomSensor(const gacommon::IODefinition& def)
{
    auto inputIdx = Rng::genChoise(def.inputs.size());
    const auto& input = def.inputs[inputIdx];

    if(std::holds_alternative<gacommon::ValueIO>(input))
    {
        return SensoricActivatorForValue{
            inputIdx,
            static_cast<SensoricActivatorForValue::Type>(Rng::genChoise(SensoricActivatorForValue::TypeSize)),
            1.0 - Rng::genReal() * 2,
            1.0 - Rng::genReal() * 2
        };
    }
    else if(std::holds_alternative<gacommon::ChoiceIO>(input))
    {
        const auto& in = std::get<gacommon::ChoiceIO>(input);
        auto details = SensoricActivatorForChoice{
            inputIdx, {}, false
        };

        details.values.resize(in.options);
        details.values[Rng::genChoise(in.options)] = 1;

        return details;
    }
    else
    {
        throw std::runtime_error("Cannot generate sensor for type: " + std::to_string(input.index()));
    }
}

ActivatorDefinition generateActivator(const gacommon::IODefinition& def)
{
    switch(Rng::genChoise(4))
    {
        case 0:
            return AlwaysActivator();

        case 1:
            return generateRandomSensor(def);

        case 2:
            return ChainActivator{};

        case 3:
            return ConsumeActivator{generatePrimitive(), generatePrimitive()};
    }

    throw std::runtime_error("Internal error");
}

ForceDefinition generateForce(const gacommon::IODefinition& def, activator)
{
    switch(Rng::genChoise(6))
    {
        case 0:
            return generateRandomManipulator(def);

        case 1:
            return CombineForce{};

        case 2:
            return SinkForce{};

        case 3:
            return ProduceForce{generatePrimitive()};

        case 4:
            return BlockForce{1};

        case 5:
            return DecomposeForce{1};
    }
    throw std::runtime_error("Internal error");
}

ForceDefinition generateRandomManipulator(const gacommon::IODefinition& def)
{
    auto outputIdx = Rng::genChoise(def.outputs.size());
    const auto& output = def.outputs[outputIdx];

    if(std::holds_alternative<gacommon::ValueIO>(output))
    {
        return SetValueManipulator{
            outputIdx,
            static_cast<SetValueManipulator::FunctionType>(Rng::genChoise(SetValueManipulator::FunctionTypeSize))
        };
    }
    else if(std::holds_alternative<gacommon::ChoiceIO>(output))
    {
        const auto& out = std::get<gacommon::ChoiceIO>(output);
        return SetChoiceManipulator{
            outputIdx,
            Rng::genChoise(out.options)
        };
    }
    else
    {
        throw std::runtime_error("Cannot generate manipulator for type: " + std::to_string(output.index()));
    }
}

void tweak(double& val, const bool isMajor)
{
    if(isMajor)
    {
        val += 1 - Rng::genReal() * 2;
    }
    else
    {
        val += 0.1 - (Rng::genReal() * 2) / 10;
    }
}

void tweak(AlwaysActivator& object, const gacommon::IODefinition& def, const bool isMajor)
{
}

void tweak(ChainActivator& object, const gacommon::IODefinition& def, const bool isMajor)
{
}

void tweak(ConsumeActivator& object, const gacommon::IODefinition& def, const bool isMajor)
{
    if(isMajor)
    {
        const bool grow = Rng::genProbability(0.5);
        if(Rng::genProbability(0.5))
        {
            if(grow || object.left.size() < 2)
            {
                object.left += generatePrimitive();
            }
            else
            {
                object.left.pop_back();
            }
        }
        else
        {
            if(grow || object.right.size() < 2)
            {
                object.right += generatePrimitive();
            }
            else
            {
                object.right.pop_back();
            }
        }
    }
    else
    {
        if(Rng::genProbability(0.5))
        {
            object.left[Rng::genChoise(object.left.size())] = generatePrimitive()[0];
        }
        else
        {
            object.right[Rng::genChoise(object.right.size())] = generatePrimitive()[0];
        }
    }
}

void tweak(SensoricActivatorForValue& object, const gacommon::IODefinition& def, const bool isMajor)
{
    tweak(object.a, isMajor);
    tweak(object.b, isMajor);
}

void tweak(SensoricActivatorForChoice& object, const gacommon::IODefinition& def, const bool isMajor)
{
    if(isMajor)
    {
        object.inversion = !object.inversion;
    }
    else
    {
        //Flip a random bit
        auto idx = Rng::genChoise(object.values.size());
        object.values[idx] = object.values[idx] == 0 ? 1 : 0;
    }
}

void tweak(SetValueManipulator& object, const gacommon::IODefinition& def, const bool isMajor)
{
    if(isMajor)
    {
        object.funcType = static_cast<SetValueManipulator::FunctionType>(Rng::genChoise(SetValueManipulator::FunctionTypeSize));
    }
}

void tweak(SetChoiceManipulator& object, const gacommon::IODefinition& def, const bool isMajor)
{
    if(isMajor)
    {
        const auto& in = std::get<gacommon::ChoiceIO>(def.outputs[object.outputIdx]);
        object.selection = Rng::genChoise(in.options);
    }
}

void tweak(CombineForce& object, const gacommon::IODefinition& def, const bool isMajor)
{
}

void tweak(SinkForce& object, const gacommon::IODefinition& def, const bool isMajor)
{
}

void tweak(ProduceForce& object, const gacommon::IODefinition& def, const bool isMajor)
{
    if(isMajor)
    {
        object.primitive = generatePrimitive();
    }
}

void tweak(DecomposeForce& object, const gacommon::IODefinition& def, const bool isMajor)
{
    if(!isMajor)
    {
        return;
    }
    const bool grow = Rng::genProbability(0.5);
    if(grow || object.pos < 2)
    {
        object.pos++;
    }
    else
    {
        object.pos--;
    }
}

void tweak(BlockForce& object, const gacommon::IODefinition& def, const bool isMajor)
{
    if(!isMajor)
    {
        return;
    }
    const bool grow = Rng::genProbability(0.5);
    if(grow || object.values < 2)
    {
        object.values++;
    }
    else
    {
        object.values--;
    }
}

void tweakMinor(BlockDefinition& block, const gacommon::IODefinition& def)
{
    std::visit([&def](auto& x){tweak(x, def, false);}, block.activator);
    std::visit([&def](auto& x){tweak(x, def, false);}, block.force);
}

void tweakMajor(BlockDefinition& block, const gacommon::IODefinition& def)
{
    std::visit([&def](auto& x){tweak(x, def, false);}, block.activator);
    std::visit([&def](auto& x){tweak(x, def, false);}, block.force);
}

std::vector<BlockDefinition> mutate(const gacommon::IODefinition& def, const std::vector<BlockDefinition>& input)
{
    class : public IMutationObserver
    {
    public:
        void onMinorTweak(const std::size_t idx) override {};
        void onMajorTweak(const std::size_t idx) override {};
        void onReplaceForce(const std::size_t idx) override {};
        void onReplaceActivator(const std::size_t idx) override {};
        void onInsertNode(const std::size_t idx) override {};
        void onCloneNode(const std::size_t idx) override {};
        void onDestroyNode() override {};
    } emptyObserver;
    return mutate(def, input, emptyObserver);
}

std::vector<BlockDefinition> mutate(const gacommon::IODefinition& def, const std::vector<BlockDefinition>& input, IMutationObserver& observer)
{
    constexpr double MINOR_TWEAK_CHANCE = 0.75;
    constexpr double MAJOR_TWEAK_CHANCE = 0.2;
    constexpr double REPLACE_FORCE_CHANCE = 0.02;
    constexpr double REPLACE_ACTIVATOR_CHANCE = 0.02;
    constexpr double INSERT_NODE_CHANCE = 0.01;
    constexpr double CLONE_NODE_CHANCE = 0.02;
    constexpr double DESTROY_NODE_CHANCE = 0.01;

    std::vector<BlockDefinition> newBlocks;

    for(const auto& b : input)
    {
        if(!Rng::genProbability(DESTROY_NODE_CHANCE) || newBlocks.empty())
        {
            auto subject = b;

            if(Rng::genProbability(CLONE_NODE_CHANCE))
            {
                newBlocks.push_back(b);
                observer.onCloneNode(newBlocks.size() - 1);
            }

            if(Rng::genProbability(INSERT_NODE_CHANCE))
            {
                newBlocks.push_back({generateActivator(def), generateForce(def)});
                observer.onInsertNode(newBlocks.size() - 1);
            }

            if(Rng::genProbability(MINOR_TWEAK_CHANCE))
            {
                tweakMinor(subject, def);
                observer.onMinorTweak(newBlocks.size() - 1);
            }
            else if(Rng::genProbability(MAJOR_TWEAK_CHANCE))
            {
                tweakMajor(subject, def);
                observer.onMajorTweak(newBlocks.size() - 1);
            }

            if(Rng::genProbability(REPLACE_ACTIVATOR_CHANCE))
            {
                observer.onReplaceActivator(newBlocks.size() - 1);
                subject.activator = generateActivator(def);
            }

            if(Rng::genProbability(REPLACE_FORCE_CHANCE))
            {
                observer.onReplaceForce(newBlocks.size() - 1);
                subject.force = generateForce(def);
            }

            newBlocks.push_back(subject);
        }
        else
        {
            observer.onDestroyNode();
        }
    }

    return newBlocks;
}

}
