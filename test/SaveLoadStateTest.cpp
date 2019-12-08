#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "neat/neat.hpp"

class SaveLoadStateTest
{
public:
   SaveLoadStateTest()
   {
      neat::Config c;

      c.numInputs = 3;
      c.numOutputs = 2;
      c.population = 100;
      c.compatibilityFactor = 3.0;
      c.inheritDisabledChance = 0.75;
      c.perturbationChance = 0.9;
      c.addNodeMutationChance = 0.05;
      c.addConnectionMutationChance = 0.05;
      c.removeConnectionMutationChance = 0.05;
      c.weightsMutationChance = 0.8;
      c.C1_C2 = 1.0;
      c.C3 = 2.0;

      mCfg = c;
   }

protected:
   neat::Config mCfg;
};

class SimpleFitnessEvaluator : public neat::IFitnessEvaluator
{
public:
    virtual neat::Fitness evaluate(const neat::Genom& g)
    {
        return mCurrent++;
    }

private:
    neat::Fitness mCurrent = 0;
};

bool isNeatSame(neat::Neat& a, neat::Neat& b)
{
    if(a.getPopulation().size() != b.getPopulation().size())
    {
        return false;
    }

    auto s1 = a.getPopulation().begin();
    auto s2 = b.getPopulation().begin();

    for(; s1 != a.getPopulation().end(); ++s1, ++s2)
    {
        if(s1->population.size() != s2->population.size())
        {
            return false;
        }

        if(s1->getSharedFitness() != s2->getSharedFitness() ||
            s1->getTotalFitness() != s2->getTotalFitness() ||
            s1->id != s2->id ||
            s1->maxFitness != s2->maxFitness)
        {
            return false;
        }

        auto p1 = s1->population.begin();
        auto p2 = s2->population.begin();

        for(; p1 != s1->population.end(); ++p1, ++p2)
        {
            if(neat::Genom::calculateDivergence(p1->genotype, p2->genotype) != 0 ||
            p1->fitness != p2->fitness)
            {
                return false;
            }
        }
    }

    return true;
}

BOOST_FIXTURE_TEST_CASE( SaveLoadTest2, SaveLoadStateTest ) 
{  
    SimpleFitnessEvaluator ev;
    neat::Neat n(mCfg, ev);

    n.step();

    n.saveState("test2.state");

    neat::Neat n1(mCfg, ev);

    n1.loadState("test2.state");

    BOOST_CHECK(isNeatSame(n, n1));
}

BOOST_FIXTURE_TEST_CASE( SaveLoadTest3, SaveLoadStateTest ) 
{  
    SimpleFitnessEvaluator ev;
    neat::Neat n(mCfg, ev);

    n.step();
    n.step();
    n.step();

    n.saveState("test3.state");

    neat::Neat n1(mCfg, ev);

    n1.loadState("test3.state");

    BOOST_CHECK(isNeatSame(n, n1));
}