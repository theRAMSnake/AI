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
        if(Rng::genProbability(0.5))
        {
            return SensoricActivatorForValue{
                inputIdx,
                static_cast<SensoricActivatorForValue::Type>(Rng::genChoise(SensoricActivatorForValue::TypeSize)),
                1.0 - Rng::genReal() * 2,
                1.0 - Rng::genReal() * 2
            };
        }
        else
        {
            return MultipleSensoricActivatorForValue {inputIdx};
        }
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
std::string randomChoice(const std::set<std::string>& src)
{
    auto pos = src.begin();
    std::advance(pos, Rng::genChoise(src.size()));
    return *pos;
}


ActivatorDefinition generateActivator(const gacommon::IODefinition& def, const std::set<std::string>& production)
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
            return ConsumeActivator{randomChoice(production), randomChoice(production)};
    }

    throw std::runtime_error("Internal error");
}

ForceDefinition generateForce(const gacommon::IODefinition& def, const ActivatorDefinition& act)
{
    const std::size_t numOptions = std::holds_alternative<ConsumeActivator>(act) ? 7 : 4;
    switch(Rng::genChoise(numOptions))
    {
        case 0:
            return generateRandomManipulator(def);

        case 1:
            return ProduceForce{generatePrimitive()};

        case 2:
            return BlockForce{1};

        case 3:
            return MultiplicationForce{2};

        case 4:
            return DecomposeForce{1};

        case 5:
            return CombineForce{};

        case 6:
            return SinkForce{};
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

ActivatorDefinition tweak(AlwaysActivator& object, const gacommon::IODefinition& def, const bool isMajor, const std::set<std::string>& production)
{
    return object;
}

ActivatorDefinition tweak(ChainActivator& object, const gacommon::IODefinition& def, const bool isMajor, const std::set<std::string>& production)
{
    return object;
}

ActivatorDefinition tweak(ConsumeActivator& object, const gacommon::IODefinition& def, const bool isMajor, const std::set<std::string>& production)
{
    if(isMajor)
    {
        if(Rng::genProbability(0.5))
        {
            object.left = randomChoice(production);
        }
        else
        {
            object.right = randomChoice(production);
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
    return object;
}

ActivatorDefinition tweak(SensoricActivatorForValue& object, const gacommon::IODefinition& def, const bool isMajor, const std::set<std::string>& production)
{
    tweak(object.a, isMajor);
    tweak(object.b, isMajor);
    return object;
}

ActivatorDefinition tweak(MultipleSensoricActivatorForValue& object, const gacommon::IODefinition& def, const bool isMajor, const std::set<std::string>& production)
{
    return object;
}

ActivatorDefinition tweak(SensoricActivatorForChoice& object, const gacommon::IODefinition& def, const bool isMajor, const std::set<std::string>& production)
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
    return object;
}

ForceDefinition tweak(SetValueManipulator& object, const gacommon::IODefinition& def, const bool isMajor)
{
    if(isMajor)
    {
        object.funcType = static_cast<SetValueManipulator::FunctionType>(Rng::genChoise(SetValueManipulator::FunctionTypeSize));
    }
    return object;
}

ForceDefinition tweak(SetChoiceManipulator& object, const gacommon::IODefinition& def, const bool isMajor)
{
    if(isMajor)
    {
        const auto& in = std::get<gacommon::ChoiceIO>(def.outputs[object.outputIdx]);
        object.selection = Rng::genChoise(in.options);
    }
    return object;
}

ForceDefinition tweak(CombineForce& object, const gacommon::IODefinition& def, const bool isMajor)
{
    return object;
}

ForceDefinition tweak(SinkForce& object, const gacommon::IODefinition& def, const bool isMajor)
{
    return object;
}

ForceDefinition tweak(ProduceForce& object, const gacommon::IODefinition& def, const bool isMajor)
{
    if(isMajor)
    {
        object.primitive = generatePrimitive();
    }

    return object;
}

ForceDefinition tweak(DecomposeForce& object, const gacommon::IODefinition& def, const bool isMajor)
{
    if(!isMajor)
    {
        return object;
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
    return object;
}

ForceDefinition tweak(BlockForce& object, const gacommon::IODefinition& def, const bool isMajor)
{
    if(!isMajor)
    {
        return object;
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
    return object;
}

ForceDefinition tweak(MultiplicationForce& object, const gacommon::IODefinition& def, const bool isMajor)
{
    if(!isMajor)
    {
        return object;
    }
    const bool grow = Rng::genProbability(0.5);
    if(grow || object.value < 2)
    {
        object.value++;
    }
    else
    {
        object.value--;
    }
    return object;
}

void tweakMinor(BlockDefinition& block, const gacommon::IODefinition& def, const std::set<std::string>& production)
{
    block.activator = std::visit([&def, &production](auto& x){return tweak(x, def, false, production);}, block.activator);
    block.force = std::visit([&def](auto& x){return tweak(x, def, false);}, block.force);
}

void tweakMajor(BlockDefinition& block, const gacommon::IODefinition& def, const std::set<std::string>& production)
{
    block.activator = std::visit([&def, &production](auto& x){return tweak(x, def, true, production);}, block.activator);
    block.force = std::visit([&def](auto& x){return tweak(x, def, true);}, block.force);
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

void populateProduction(const std::vector<BlockDefinition>& input, std::set<std::string>& out)
{
    for(const auto& block : input)
    {
        if(std::holds_alternative<ProduceForce>(block.force))
        {
            out.insert(std::get<ProduceForce>(block.force).primitive);
        }
        else if(std::holds_alternative<CombineForce>(block.force))
        {
            out.insert(std::get<ConsumeActivator>(block.activator).left + std::get<ConsumeActivator>(block.activator).right);
        }
        else if(std::holds_alternative<DecomposeForce>(block.force))
        {
            auto pos = std::get<DecomposeForce>(block.force).pos;
            auto act = std::get<ConsumeActivator>(block.activator);
            if(pos < act.left.size())
            {
                out.insert(act.left.substr(0, pos));
                out.insert(act.left.substr(pos, std::string::npos));
            }
            else
            {
                out.insert(act.left);
            }
            if(pos < act.right.size())
            {
                out.insert(act.right.substr(0, pos));
                out.insert(act.right.substr(pos, std::string::npos));
            }
            else
            {
                out.insert(act.right);
            }
        }
    }
    if(out.empty())
    {
        out.insert(generatePrimitive());
    }
}

std::vector<BlockDefinition> mutate(const gacommon::IODefinition& def, const std::vector<BlockDefinition>& input, IMutationObserver& observer)
{
    constexpr double MINOR_TWEAK_CHANCE = 0.75;
    constexpr double MAJOR_TWEAK_CHANCE = 0.1;
    constexpr double REPLACE_FORCE_CHANCE = 0.02;
    constexpr double REPLACE_ACTIVATOR_CHANCE = 0.02;
    constexpr double INSERT_NODE_CHANCE = 0.01;
    constexpr double CLONE_NODE_CHANCE = 0.01;
    constexpr double DESTROY_NODE_CHANCE = 0.02;

    std::vector<BlockDefinition> newBlocks;
    std::set<std::string> production;
    populateProduction(input, production);

    for(const auto& b : input)
    {
        //if(!Rng::genProbability(DESTROY_NODE_CHANCE) || newBlocks.empty())
        if(true)
        {
            auto subject = b;

            if(Rng::genProbability(CLONE_NODE_CHANCE))
            {
                newBlocks.push_back(b);
                observer.onCloneNode(newBlocks.size() - 1);
            }

            if(Rng::genProbability(INSERT_NODE_CHANCE))
            {
                auto activator = generateActivator(def, production);
                newBlocks.push_back({activator, generateForce(def, activator)});
                observer.onInsertNode(newBlocks.size() - 1);
            }

            if(Rng::genProbability(MINOR_TWEAK_CHANCE))
            {
                tweakMinor(subject, def, production);
                observer.onMinorTweak(newBlocks.size() - 1);
            }

            if(Rng::genProbability(MAJOR_TWEAK_CHANCE))
            {
                tweakMajor(subject, def, production);
                observer.onMajorTweak(newBlocks.size() - 1);
            }

            if(Rng::genProbability(REPLACE_ACTIVATOR_CHANCE))
            {
                observer.onReplaceActivator(newBlocks.size() - 1);
                if(!std::holds_alternative<ConsumeActivator>(subject.activator))
                {
                    subject.activator = generateActivator(def, production);
                }
            }

            if(Rng::genProbability(REPLACE_FORCE_CHANCE))
            {
                observer.onReplaceForce(newBlocks.size() - 1);
                subject.force = generateForce(def, subject.activator);
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
