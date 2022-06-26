
#include "gacommon/IPlayground.hpp"
#include <variant>
#define BOOST_TEST_DYN_LINK
#define TEST
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

BOOST_FIXTURE_TEST_CASE( Test1000Generations, SnakeGATest )
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
            std::cout << "Mutation stats for 1000 generations:" << std::endl;
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

    for(int i = 0; i < 1000; ++i)
    {
        blocks = mutate(def, blocks, testObserver);
        snake4::createAgentImpl(def, blocks)->run(def.inputs, def.outputs);
    }

    BOOST_CHECK_EQUAL(1 + testObserver.mutationStats.numInserts + testObserver.mutationStats.numClone - testObserver.mutationStats.numDestroy, blocks.size());

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
    for(int i = 0; i < 1000; ++i)
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
