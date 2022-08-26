
#include "gacommon/IPlayground.hpp"
#include <variant>
#define BOOST_TEST_DYN_LINK
#define TEST
#define BOOST_TEST_MODULE 1
#include <boost/test/unit_test.hpp>
#include "snake4/system.hpp"
#include "snake4/pop.hpp"
#include "snake4/mutations.hpp"

class SnakeGATest
{
protected:
};

BOOST_FIXTURE_TEST_CASE( TestAlwaysActivatorAndSetManipulator, SnakeGATest )
{
    using namespace snake4;
    gacommon::IODefinition def{
        {},
        {gacommon::ValueIO{0}, gacommon::ValueIO{0}, gacommon::ValueIO{0}}
    };
    std::vector<BlockDefinition> blocks = {
        {AlwaysActivator(), SetValueManipulator{0, SetValueManipulator::FunctionType::One}},
        {AlwaysActivator(), SetValueManipulator{1, SetValueManipulator::FunctionType::One}},
        {AlwaysActivator(), SetValueManipulator{2, SetValueManipulator::FunctionType::One}},
    };

    snake4::createAgentImpl(def, blocks)->run(def.inputs, def.outputs);

    BOOST_CHECK_EQUAL(1, std::get<0>(def.outputs[0]).value);
    BOOST_CHECK_EQUAL(1, std::get<0>(def.outputs[1]).value);
    BOOST_CHECK_EQUAL(1, std::get<0>(def.outputs[2]).value);
}

BOOST_FIXTURE_TEST_CASE( TestMultipleSensoricValue, SnakeGATest )
{
    using namespace snake4;

    gacommon::IODefinition def{
        {gacommon::ValueIO{5}, gacommon::ValueIO{0}},
        {gacommon::ValueIO{0}, gacommon::ValueIO{0}}
    };

    std::vector<BlockDefinition> blocks = {
        {MultipleSensoricActivatorForValue{0}, SetValueManipulator{0, SetValueManipulator::FunctionType::Inc}},
        {MultipleSensoricActivatorForValue{1}, SetValueManipulator{1, SetValueManipulator::FunctionType::Inc}},
    };

    snake4::createAgentImpl(def, blocks)->run(def.inputs, def.outputs);

    BOOST_CHECK_EQUAL(5, std::get<0>(def.outputs[0]).value);
    BOOST_CHECK_EQUAL(0, std::get<0>(def.outputs[1]).value);
}

BOOST_FIXTURE_TEST_CASE( TestSensoricValue, SnakeGATest )
{
    using namespace snake4;

    {
        gacommon::IODefinition def{
            {gacommon::ValueIO{0.25}, gacommon::ValueIO{0}, gacommon::ValueIO{-0.33}},
            {gacommon::ValueIO{0}, gacommon::ValueIO{0}, gacommon::ValueIO{0}}
        };

        std::vector<BlockDefinition> blocks = {
            {SensoricActivatorForValue{0, SensoricActivatorForValue::Type::Above, 0.1}, SetValueManipulator{0, SetValueManipulator::FunctionType::One}},
            {SensoricActivatorForValue{1, SensoricActivatorForValue::Type::Above, 0.1}, SetValueManipulator{1, SetValueManipulator::FunctionType::One}},
            {SensoricActivatorForValue{2, SensoricActivatorForValue::Type::Above, 0.1}, SetValueManipulator{2, SetValueManipulator::FunctionType::One}},
        };

        snake4::createAgentImpl(def, blocks)->run(def.inputs, def.outputs);

        BOOST_CHECK_EQUAL(1, std::get<0>(def.outputs[0]).value);
        BOOST_CHECK_EQUAL(0, std::get<0>(def.outputs[1]).value);
        BOOST_CHECK_EQUAL(0, std::get<0>(def.outputs[2]).value);
    }

    {
        gacommon::IODefinition def{
            {gacommon::ValueIO{0.25}, gacommon::ValueIO{0}, gacommon::ValueIO{-0.33}},
            {gacommon::ValueIO{0}, gacommon::ValueIO{0}, gacommon::ValueIO{0}}
        };

        std::vector<BlockDefinition> blocks = {
            {SensoricActivatorForValue{0, SensoricActivatorForValue::Type::Below, 0.1}, SetValueManipulator{0, SetValueManipulator::FunctionType::One}},
            {SensoricActivatorForValue{1, SensoricActivatorForValue::Type::Below, 0.1}, SetValueManipulator{1, SetValueManipulator::FunctionType::One}},
            {SensoricActivatorForValue{2, SensoricActivatorForValue::Type::Below, 0.1}, SetValueManipulator{2, SetValueManipulator::FunctionType::One}},
        };

        snake4::createAgentImpl(def, blocks)->run(def.inputs, def.outputs);

        BOOST_CHECK_EQUAL(0, std::get<0>(def.outputs[0]).value);
        BOOST_CHECK_EQUAL(1, std::get<0>(def.outputs[1]).value);
        BOOST_CHECK_EQUAL(1, std::get<0>(def.outputs[2]).value);
    }

    {
        gacommon::IODefinition def{
            {gacommon::ValueIO{0.25}, gacommon::ValueIO{0}, gacommon::ValueIO{-0.33}},
            {gacommon::ValueIO{0}, gacommon::ValueIO{0}, gacommon::ValueIO{0}}
        };

        std::vector<BlockDefinition> blocks = {
            {SensoricActivatorForValue{0, SensoricActivatorForValue::Type::InRange, -0.1, 0.1}, SetValueManipulator{0, SetValueManipulator::FunctionType::One}},
            {SensoricActivatorForValue{1, SensoricActivatorForValue::Type::InRange, -0.1, 0.1}, SetValueManipulator{1, SetValueManipulator::FunctionType::One}},
            {SensoricActivatorForValue{2, SensoricActivatorForValue::Type::InRange, -0.1, 0.1}, SetValueManipulator{2, SetValueManipulator::FunctionType::One}},
        };

        snake4::createAgentImpl(def, blocks)->run(def.inputs, def.outputs);

        BOOST_CHECK_EQUAL(0, std::get<0>(def.outputs[0]).value);
        BOOST_CHECK_EQUAL(1, std::get<0>(def.outputs[1]).value);
        BOOST_CHECK_EQUAL(0, std::get<0>(def.outputs[2]).value);
    }
    {
        gacommon::IODefinition def{
            {gacommon::ValueIO{0.25}, gacommon::ValueIO{0}, gacommon::ValueIO{-0.33}},
            {gacommon::ValueIO{0}, gacommon::ValueIO{0}, gacommon::ValueIO{0}}
        };

        std::vector<BlockDefinition> blocks = {
            {SensoricActivatorForValue{0, SensoricActivatorForValue::Type::NotInRange, -0.1, 0.1}, SetValueManipulator{0, SetValueManipulator::FunctionType::One}},
            {SensoricActivatorForValue{1, SensoricActivatorForValue::Type::NotInRange, -0.1, 0.1}, SetValueManipulator{1, SetValueManipulator::FunctionType::One}},
            {SensoricActivatorForValue{2, SensoricActivatorForValue::Type::NotInRange, -0.1, 0.1}, SetValueManipulator{2, SetValueManipulator::FunctionType::One}},
        };

        snake4::createAgentImpl(def, blocks)->run(def.inputs, def.outputs);

        BOOST_CHECK_EQUAL(1, std::get<0>(def.outputs[0]).value);
        BOOST_CHECK_EQUAL(0, std::get<0>(def.outputs[1]).value);
        BOOST_CHECK_EQUAL(1, std::get<0>(def.outputs[2]).value);
    }
}

BOOST_FIXTURE_TEST_CASE( TestSensoricChoice, SnakeGATest )
{
    using namespace snake4;
    {
        gacommon::IODefinition def{
            {gacommon::ChoiceIO{5, 1}, gacommon::ChoiceIO{4, 2}, gacommon::ChoiceIO{2, 0}},
            {gacommon::ValueIO{0}, gacommon::ValueIO{0}, gacommon::ValueIO{0}}
        };

        std::vector<BlockDefinition> blocks = {
            {SensoricActivatorForChoice{0, {0, 1, 0, 0, 0}, false}, SetValueManipulator{0, SetValueManipulator::FunctionType::One}},
            {SensoricActivatorForChoice{1, {0, 1, 0, 1}, false}, SetValueManipulator{1, SetValueManipulator::FunctionType::One}},
            {SensoricActivatorForChoice{2, {0, 1}, true}, SetValueManipulator{2, SetValueManipulator::FunctionType::One}},
        };

        snake4::createAgentImpl(def, blocks)->run(def.inputs, def.outputs);

        BOOST_CHECK_EQUAL(1, std::get<0>(def.outputs[0]).value);
        BOOST_CHECK_EQUAL(0, std::get<0>(def.outputs[1]).value);
        BOOST_CHECK_EQUAL(1, std::get<0>(def.outputs[2]).value);
    }
}

BOOST_FIXTURE_TEST_CASE( TestValueManipulator, SnakeGATest )
{
    using namespace snake4;
    gacommon::IODefinition def{
        {},
        {gacommon::ValueIO{-1}, gacommon::ValueIO{-1}, gacommon::ValueIO{-1}, gacommon::ValueIO{-1}}
    };
    std::vector<BlockDefinition> blocks = {
        {AlwaysActivator(), SetValueManipulator{0, SetValueManipulator::FunctionType::One}},
        {AlwaysActivator(), SetValueManipulator{1, SetValueManipulator::FunctionType::Zero}},
        {AlwaysActivator(), SetValueManipulator{2, SetValueManipulator::FunctionType::Dec}},
        {AlwaysActivator(), SetValueManipulator{3, SetValueManipulator::FunctionType::Inc}},
    };

    snake4::createAgentImpl(def, blocks)->run(def.inputs, def.outputs);

    BOOST_CHECK_EQUAL(1, std::get<0>(def.outputs[0]).value);
    BOOST_CHECK_EQUAL(0, std::get<0>(def.outputs[1]).value);
    BOOST_CHECK_EQUAL(-2, std::get<0>(def.outputs[2]).value);
    BOOST_CHECK_EQUAL(0, std::get<0>(def.outputs[3]).value);
}
BOOST_FIXTURE_TEST_CASE( TestConsume, SnakeGATest )
{
    using namespace snake4;
    gacommon::IODefinition def{
        {},
        {gacommon::ValueIO{-1}}
    };
    std::vector<BlockDefinition> blocks = {
        {AlwaysActivator(), ProduceForce{"A"}},
        {AlwaysActivator(), ProduceForce{"B"}},
        {AlwaysActivator(), ProduceForce{"A"}},
        {AlwaysActivator(), ProduceForce{"B"}},
        {ConsumeActivator{"A", "B"}, SetValueManipulator{0, SetValueManipulator::FunctionType::One}},
        {ConsumeActivator{"B", "B"}, SetValueManipulator{0, SetValueManipulator::FunctionType::Zero}},
        {ConsumeActivator{"A", "B"}, SetValueManipulator{0, SetValueManipulator::FunctionType::One}},
        {ConsumeActivator{"A", "B"}, SetValueManipulator{0, SetValueManipulator::FunctionType::Zero}},
        {ConsumeActivator{"B", "A"}, SetValueManipulator{0, SetValueManipulator::FunctionType::Zero}},
    };

    snake4::createAgentImpl(def, blocks)->run(def.inputs, def.outputs);

    BOOST_CHECK_EQUAL(1, std::get<0>(def.outputs[0]).value);
}
BOOST_FIXTURE_TEST_CASE( TestSink, SnakeGATest )
{
    using namespace snake4;
    gacommon::IODefinition def{
        {},
        {gacommon::ValueIO{-1}}
    };
    std::vector<BlockDefinition> blocks = {
        {AlwaysActivator(), ProduceForce{"A"}},
        {AlwaysActivator(), ProduceForce{"B"}},
        {ConsumeActivator{"A", "B"}, SinkForce{}},
        {ConsumeActivator{"A", "B"}, SinkForce{}},
        {ConsumeActivator{"B", "A"}, SetValueManipulator{0, SetValueManipulator::FunctionType::Zero}},
    };

    snake4::createAgentImpl(def, blocks)->run(def.inputs, def.outputs);

    BOOST_CHECK_EQUAL(-1, std::get<0>(def.outputs[0]).value);
}
BOOST_FIXTURE_TEST_CASE( TestBlock, SnakeGATest )
{
    using namespace snake4;
    gacommon::IODefinition def{
        {},
        {gacommon::ValueIO{-1}}
    };
    std::vector<BlockDefinition> blocks = {
        {AlwaysActivator(), BlockForce{4}},
        {AlwaysActivator(), ProduceForce{"A"}},
        {AlwaysActivator(), ProduceForce{"B"}},
        {AlwaysActivator(), ProduceForce{"A"}},
        {AlwaysActivator(), ProduceForce{"B"}},
        {ConsumeActivator{"B", "A"}, SetValueManipulator{0, SetValueManipulator::FunctionType::Zero}},
    };

    snake4::createAgentImpl(def, blocks)->run(def.inputs, def.outputs);

    BOOST_CHECK_EQUAL(-1, std::get<0>(def.outputs[0]).value);
}
BOOST_FIXTURE_TEST_CASE( TestMultiplicationForce, SnakeGATest )
{
    using namespace snake4;
    gacommon::IODefinition def{
        {},
        {gacommon::ValueIO{0}}
    };
    std::vector<BlockDefinition> blocks = {
        {AlwaysActivator(), ProduceForce{"A"}},
        {AlwaysActivator(), ProduceForce{"B"}},
        {AlwaysActivator(), MultiplicationForce{4}},
        {ConsumeActivator{"B", "A"}, SetValueManipulator{0, SetValueManipulator::FunctionType::Inc}},
    };

    snake4::createAgentImpl(def, blocks)->run(def.inputs, def.outputs);

    BOOST_CHECK_EQUAL(2, std::get<0>(def.outputs[0]).value);
}
BOOST_FIXTURE_TEST_CASE( TestCombine, SnakeGATest )
{
    using namespace snake4;
    gacommon::IODefinition def{
        {},
        {gacommon::ValueIO{-1}}
    };
    std::vector<BlockDefinition> blocks = {
        {AlwaysActivator(), ProduceForce{"A"}},
        {AlwaysActivator(), ProduceForce{"A"}},
        {AlwaysActivator(), ProduceForce{"B"}},
        {AlwaysActivator(), ProduceForce{"A"}},
        {AlwaysActivator(), ProduceForce{"B"}},
        {ConsumeActivator{"A", "B"}, CombineForce{}},
        {ConsumeActivator{"B", "A"}, CombineForce{}},
        {ConsumeActivator{"AB", "BA"}, CombineForce{}},
        {ConsumeActivator{"ABBA", "A"}, SetValueManipulator{0, SetValueManipulator::FunctionType::One}},
        {ConsumeActivator{"ABBA", "A"}, SetValueManipulator{0, SetValueManipulator::FunctionType::Zero}},
    };

    snake4::createAgentImpl(def, blocks)->run(def.inputs, def.outputs);

    BOOST_CHECK_EQUAL(1, std::get<0>(def.outputs[0]).value);
}
BOOST_FIXTURE_TEST_CASE( TestDecompose, SnakeGATest )
{
    using namespace snake4;
    gacommon::IODefinition def{
        {},
        {gacommon::ValueIO{-1}}
    };
    std::vector<BlockDefinition> blocks = {
        {AlwaysActivator(), ProduceForce{"A"}},
        {AlwaysActivator(), ProduceForce{"B"}},
        {AlwaysActivator(), ProduceForce{"A"}},
        {AlwaysActivator(), ProduceForce{"B"}},
        {ConsumeActivator{"A", "B"}, CombineForce{}},
        {ConsumeActivator{"B", "A"}, CombineForce{}},
        {ConsumeActivator{"AB", "BA"}, CombineForce{}},
        {ConsumeActivator{"ABBA", "A"}, DecomposeForce{2}},
        {ConsumeActivator{"AB", "BA"}, SetValueManipulator{0, SetValueManipulator::FunctionType::One}},
        {ConsumeActivator{"A", "B"}, SetValueManipulator{0, SetValueManipulator::FunctionType::Inc}},
        {ConsumeActivator{"A", "B"}, SetValueManipulator{0, SetValueManipulator::FunctionType::Inc}},
        {ConsumeActivator{"A", "B"}, SetValueManipulator{0, SetValueManipulator::FunctionType::Zero}},
    };

    snake4::createAgentImpl(def, blocks)->run(def.inputs, def.outputs);

    BOOST_CHECK_EQUAL(3, std::get<0>(def.outputs[0]).value);
}
BOOST_FIXTURE_TEST_CASE( TestChoiceManipulator, SnakeGATest )
{
    using namespace snake4;
    gacommon::IODefinition def{
        {},
        {gacommon::ChoiceIO{5, 0}, gacommon::ChoiceIO{3, 0}, gacommon::ChoiceIO{4, 0}}
    };
    std::vector<BlockDefinition> blocks = {
        {AlwaysActivator(), SetChoiceManipulator{0, 3}},
        {AlwaysActivator(), SetChoiceManipulator{0, 2}},
        {AlwaysActivator(), SetChoiceManipulator{0, 3}},
        {AlwaysActivator(), SetChoiceManipulator{1, 1}},
        {AlwaysActivator(), SetChoiceManipulator{1, 1}},
    };

    snake4::createAgentImpl(def, blocks)->run(def.inputs, def.outputs);

    BOOST_CHECK_EQUAL(3, std::get<1>(def.outputs[0]).selection);
    BOOST_CHECK_EQUAL(1, std::get<1>(def.outputs[1]).selection);
    BOOST_CHECK_EQUAL(0, std::get<1>(def.outputs[2]).selection);
}

template<class T>
bool sanityCheck(const T& x)
{
    return true;
}

bool sanityCheck(const snake4::ConsumeActivator& x)
{
    return !x.left.empty() && !x.right.empty();
}
bool sanityCheck(const snake4::DecomposeForce& x)
{
    return x.pos > 0 && x.pos < 1000000;
}
bool sanityCheck(const snake4::BlockForce& x)
{
    return x.values > 0 && x.values < 1000000;
}
bool sanityCheck(const snake4::MultiplicationForce& x)
{
    return x.value > 0 && x.value < 1000000;
}

bool sanityCheck(const snake4::ProduceForce& x)
{
    return x.primitive.size() == 1 && (x.primitive[0] == 'A' ||
        x.primitive[0] == 'B' ||
        x.primitive[0] == 'C' ||
        x.primitive[0] == 'D' ||
        x.primitive[0] == 'E' ||
        x.primitive[0] == 'F');
}
void sanityCheck(const snake4::BlockDefinition& block)
{
    if(std::holds_alternative<snake4::CombineForce>(block.force) ||
        std::holds_alternative<snake4::SinkForce>(block.force) ||
        std::holds_alternative<snake4::DecomposeForce>(block.force))
    {
        BOOST_CHECK(std::holds_alternative<snake4::ConsumeActivator>(block.activator));
    }

    std::visit([](auto&& arg){BOOST_CHECK(sanityCheck(arg));}, block.activator);
    std::visit([](auto&& arg){BOOST_CHECK(sanityCheck(arg));}, block.force);
}

BOOST_FIXTURE_TEST_CASE( Test500Generations, SnakeGATest )
{
    using namespace snake4;
    gacommon::IODefinition def{
        {gacommon::ValueIO{}, gacommon::ChoiceIO{4, 0}, gacommon::ValueIO{}, gacommon::ChoiceIO{2, 0}},
        {gacommon::ChoiceIO{5, 0}, gacommon::ChoiceIO{3, 0}, gacommon::ValueIO{}, gacommon::ValueIO{}}
    };
    std::vector<BlockDefinition> blocks = Pop::createMinimal(def).blocks;

    class : public IMutationObserver
    {
    public:
        void onMinorTweak(const std::size_t idx) override
        {
            mutationStats.numMinor++;
        }
        void onMajorTweak(const std::size_t idx) override
        {
            mutationStats.numMajor++;
        }
        void onReplaceForce(const std::size_t idx) override
        {
            mutationStats.numReplaceForce++;
        }
        void onReplaceActivator(const std::size_t idx) override
        {
            mutationStats.numReplaceActivator++;
        }
        void onInsertNode(const std::size_t idx) override
        {
            mutationStats.numInserts++;
        }
        void onCloneNode(const std::size_t idx) override
        {
            mutationStats.numClone++;
        }
        void onDestroyNode() override
        {
            mutationStats.numDestroy++;
        }
        void printStats()
        {
            std::cout << "Mutation stats for 500 generations:" << std::endl;
            std::cout << "Minor tweaks: " << mutationStats.numMinor << std::endl;
            std::cout << "Major tweaks: " << mutationStats.numMajor << std::endl;
            std::cout << "Replace force: " << mutationStats.numReplaceForce << std::endl;
            std::cout << "Replace activator: " << mutationStats.numReplaceActivator << std::endl;
            std::cout << "Inserts: " << mutationStats.numInserts << std::endl;
            std::cout << "Clone: " << mutationStats.numClone << std::endl;
            std::cout << "Destroy: " << mutationStats.numDestroy << std::endl;
        }

        struct
        {
            int numMinor = 0;
            int numMajor = 0;
            int numReplaceForce = 0;
            int numReplaceActivator = 0;
            int numInserts = 0;
            int numDestroy = 0;
            int numClone = 0;
        } mutationStats;

    } testObserver;

    for(int i = 0; i < 500; ++i)
    {
        blocks = mutate(def, blocks, testObserver);
        snake4::createAgentImpl(def, blocks)->run(def.inputs, def.outputs);
        if(i % 100 == 0)
        {
            std::cout << "=" << std::endl;
        }
    }

    BOOST_CHECK_EQUAL(1 + testObserver.mutationStats.numInserts + testObserver.mutationStats.numClone - testObserver.mutationStats.numDestroy, blocks.size());

    for(const auto& b : blocks)
    {
        sanityCheck(b);
    }

    testObserver.printStats();
}

template<class T1, class T2>
bool cmp(const T1& a, const T2& b)
{
    throw std::runtime_error("Internal error: cmp called for 2 different types");
}

bool cmp(const snake4::AlwaysActivator& a, const snake4::AlwaysActivator& b)
{
    return true;
}
bool cmp(const snake4::SensoricActivatorForValue& a, const snake4::SensoricActivatorForValue& b)
{
    return a.a == b.a && a.b == b.b && a.inputIdx == b.inputIdx && a.type == b.type;
}
bool cmp(const snake4::MultipleSensoricActivatorForValue& a, const snake4::MultipleSensoricActivatorForValue& b)
{
    return a.inputIdx == b.inputIdx;
}
bool cmp(const snake4::SensoricActivatorForChoice& a, const snake4::SensoricActivatorForChoice& b)
{
    return a.inputIdx == b.inputIdx && a.inversion == b.inversion && a.values == b.values;
}
bool cmp(const snake4::SetValueManipulator& a, const snake4::SetValueManipulator& b)
{
    return a.funcType == b.funcType && a.outputIdx == b.outputIdx;
}
bool cmp(const snake4::SetChoiceManipulator& a, const snake4::SetChoiceManipulator& b)
{
    return a.selection == b.selection && a.outputIdx == b.outputIdx;
}
bool cmp(const snake4::ConsumeActivator& a, const snake4::ConsumeActivator& b)
{
    return a.left == b.left && a.right == b.right;
}
bool cmp(const snake4::ChainActivator& a, const snake4::ChainActivator& b)
{
    return true;
}
bool cmp(const snake4::CombineForce& a, const snake4::CombineForce& b)
{
    return true;
}
bool cmp(const snake4::SinkForce& a, const snake4::SinkForce& b)
{
    return true;
}
bool cmp(const snake4::ProduceForce& a, const snake4::ProduceForce& b)
{
    return a.primitive == b.primitive;
}
bool cmp(const snake4::DecomposeForce& a, const snake4::DecomposeForce& b)
{
    return a.pos == b.pos;
}
bool cmp(const snake4::BlockForce& a, const snake4::BlockForce& b)
{
    return a.values == b.values;
}
bool cmp(const snake4::MultiplicationForce& a, const snake4::MultiplicationForce& b)
{
    return a.value == b.value;
}

bool blockCmp(const snake4::BlockDefinition& a, const snake4::BlockDefinition& b)
{
    if(a.activator.index() == b.activator.index())
    {
        bool result = false;
        std::visit([&result, &b](const auto& arg){
                std::visit([&result, &arg](const auto& arg2){
                     result = cmp(arg, arg2);
                }, b.activator);
            }, a.activator);

        return result;
    }
    else
    {
        return false;
    }

    if(a.force.index() == b.force.index())
    {
        bool result = false;
        std::visit([&result, &b](const auto& arg){
                std::visit([&result, &arg](const auto& arg2){
                     result = cmp(arg, arg2);
                }, b.force);
            }, a.force);

        return result;

    }
    else
    {
        return false;
    }
}

BOOST_FIXTURE_TEST_CASE( TestSerialization, SnakeGATest )
{
    using namespace snake4;
    gacommon::IODefinition def{
        {gacommon::ValueIO{}, gacommon::ChoiceIO{4, 0}, gacommon::ValueIO{}, gacommon::ChoiceIO{2, 0}},
        {gacommon::ChoiceIO{5, 0}, gacommon::ChoiceIO{3, 0}, gacommon::ValueIO{}, gacommon::ValueIO{}}
    };

    Pop pop = Pop::createMinimal(def);
    for(int i = 0; i < 100; ++i)
    {
        pop = pop.cloneMutated(def);
    }

    boost::property_tree::ptree ser;
    pop.saveState(ser);

    auto pop2 = Pop::loadState(ser);
    BOOST_CHECK_EQUAL(pop.fitness, pop2.fitness);
    BOOST_CHECK_EQUAL(pop.blocks.size(), pop2.blocks.size());
    for(std::size_t i = 0; i < pop.blocks.size(); ++i)
    {
        BOOST_CHECK(blockCmp(pop.blocks[i], pop2.blocks[i]));
    }
}
