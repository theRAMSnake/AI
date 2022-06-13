#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "neat/genom.hpp"

class CrossoverTest
{
public:
   CrossoverTest()
   {
        
   }

protected:
   neat::v2::Genom createSampleGenom()
   {
       return neat::v2::Genom::createMinimal(3, 2, mHistory, true);
   }

   neat::InnovationHistory mHistory;
};

BOOST_FIXTURE_TEST_CASE( Test1, CrossoverTest ) 
{  
    //Same genom
    neat::v2::Genom a = createSampleGenom();

    auto c = neat::v2::Genom::crossover(a, a, 1.0, 1.0);

    BOOST_CHECK_EQUAL(a.getComplexity(), c.getComplexity());
    for(std::size_t i = 0; i < a.getComplexity(); ++i)
    {
        BOOST_CHECK_EQUAL(a[i].dstNodeId, c[i].dstNodeId);
        BOOST_CHECK_EQUAL(a[i].innovationNumber, c[i].innovationNumber);
        BOOST_CHECK_EQUAL(a[i].srcNodeId, c[i].srcNodeId);
        BOOST_CHECK_EQUAL(a[i].weight, c[i].weight);
    }
}

BOOST_FIXTURE_TEST_CASE( Test2, CrossoverTest ) 
{
    //Same innovation numbers, only weights different
    neat::v2::Genom a = createSampleGenom();
    neat::v2::Genom b = createSampleGenom();

    auto c = neat::v2::Genom::crossover(a, b, 1.0, 1.0);

    BOOST_CHECK_EQUAL(a.getComplexity(), c.getComplexity());
    for(std::size_t i = 0; i < a.getComplexity(); ++i)
    {
        BOOST_CHECK_EQUAL(a[i].dstNodeId, c[i].dstNodeId);
        BOOST_CHECK_EQUAL(a[i].innovationNumber, c[i].innovationNumber);
        BOOST_CHECK_EQUAL(a[i].srcNodeId, c[i].srcNodeId);
        BOOST_CHECK(a[i].weight == c[i].weight || b[i].weight == c[i].weight);
    }
}

BOOST_FIXTURE_TEST_CASE( Test3, CrossoverTest ) 
{
    neat::v2::MutationConfig cfg;
    cfg.addNodeMutationChance = 1.0;

    //Same innovation numbers, different weights, different tails
    neat::v2::Genom a = createSampleGenom();
    a.mutate(cfg, mHistory);

    neat::v2::Genom b = createSampleGenom();
    b.mutate(cfg, mHistory);

    {
        auto c = neat::v2::Genom::crossover(a, b, 1.0, 0.0);

        BOOST_CHECK_EQUAL(a.getComplexity(), c.getComplexity());
        for(std::size_t i = 0; i < a.getComplexity(); ++i)
        {
            BOOST_CHECK_EQUAL(a[i].dstNodeId, c[i].dstNodeId);
            BOOST_CHECK_EQUAL(a[i].innovationNumber, c[i].innovationNumber);
            BOOST_CHECK_EQUAL(a[i].srcNodeId, c[i].srcNodeId);
        }
    }
    {
        auto c = neat::v2::Genom::crossover(a, b, 0.0, 1.0);

        BOOST_CHECK_EQUAL(b.getComplexity(), c.getComplexity());
        for(std::size_t i = 0; i < b.getComplexity(); ++i)
        {
            BOOST_CHECK_EQUAL(b[i].dstNodeId, c[i].dstNodeId);
            BOOST_CHECK_EQUAL(b[i].innovationNumber, c[i].innovationNumber);
            BOOST_CHECK_EQUAL(b[i].srcNodeId, c[i].srcNodeId);
        }
    }
}
