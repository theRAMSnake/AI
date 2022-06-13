#define BOOST_TEST_DYN_LINK
#define TEST
#include <boost/test/unit_test.hpp>
#include "gacommon/natural_selection.hpp"

class NaturalSelectionTest
{

protected:
};

class TestAgent : public gacommon::IAgent
{
public:
    TestAgent(int value)
    {
        mValue = value;
    }
    void reset(){}
    void run(const std::vector<gacommon::IOElement>& inputs, std::vector<gacommon::IOElement>& output)
    {
       std::get<0>(output[0]).value = mValue;
    }
    void toBinaryStream(std::ofstream& stream) const{}

private:
   int mValue = 0;
};

class TestPop
{
public:
    TestPop(int value)
    {
        mValue = value;
    }

    TestPop cloneMutated(const gacommon::IODefinition& io) const
    {
        return TestPop{Rng::genProbability(0.5) ? mValue + 1 : mValue - 1};
    }

    static TestPop createMinimal(const gacommon::IODefinition& io)
    {
        return TestPop{static_cast<int>(Rng::genChoise(10000)) - 5000};
    }

    std::unique_ptr<gacommon::IAgent> createAgent(const gacommon::IODefinition& io) const
    {
        return std::make_unique<TestAgent>(mValue);
    }

    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int version)
    {
    }

    gacommon::Fitness fitness = 0;

private:
    int mValue = 0;
};

class TestFitnessEvaluator : public gacommon::IFitnessEvaluator
{
    gacommon::Fitness evaluate(gacommon::IAgent& agent) override
    {
        std::vector<gacommon::IOElement> inputs;
        std::vector<gacommon::IOElement> outputs{
            gacommon::ValueIO {0}
        };

        agent.run(inputs, outputs);

        return std::max(0, 5000 - std::abs(3500 - static_cast<int>(std::get<0>(outputs[0]).value)));
    }
};

BOOST_FIXTURE_TEST_CASE( TestConvergence, NaturalSelectionTest )
{
    Rng::seed(static_cast<unsigned int>(std::time(0)));

    //Idea of test is to guess number 3500, by mutating internal number of pop as of + or - random number
    //Fitness is the distance between 3500 and pop internal number
    gacommon::Config cfg{100, 2, 0.25, 3};
    TestFitnessEvaluator eval;
    gacommon::IODefinition def;
    gacommon::NaturalSelection<TestPop> algo(cfg, def, eval);

    bool converged = false;
    for(int i = 0; i < 10000; ++i)
    {
        algo.step();
        if(algo.getPopulation()[0].fitness == 5000)
        {
            std::cout << "Converged after " << i << " generations" << std::endl;
            converged = true;
            break;
        }
    }

    BOOST_REQUIRE(converged);
}

