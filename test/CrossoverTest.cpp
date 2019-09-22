#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE a
#include <boost/test/unit_test.hpp>
#include "neat/genom.hpp"

class CrossoverTest
{
public:
   CrossoverTest()
   {
      
   }

protected:
   neat::Genom createSampleGenom()
   {
       return neat::createMinimalGenom(3, 2);
   }
};

BOOST_FIXTURE_TEST_CASE( Test1, CrossoverTest ) 
{  
    //Same genom
    neat::Genom a = createSampleGenom();

    auto c = neat::crossover(a, a, 1.0, 1.0);

    BOOST_CHECK_EQUAL(a.genes.size(), c.genes.size());
    for(std::size_t i = 0; i < a.genes.size(); ++i)
    {
        BOOST_CHECK_EQUAL(a.genes[i].dstNodeId, c.genes[i].dstNodeId);
        BOOST_CHECK_EQUAL(a.genes[i].innovationNumber, c.genes[i].innovationNumber);
        BOOST_CHECK_EQUAL(a.genes[i].srcNodeId, c.genes[i].srcNodeId);
        BOOST_CHECK_EQUAL(a.genes[i].weight, c.genes[i].weight);
        BOOST_CHECK_EQUAL(a.genes[i].enabled, c.genes[i].enabled);
    }
}

BOOST_FIXTURE_TEST_CASE( Test2, CrossoverTest ) 
{
    //Same innovation numbers, only weights different
    neat::Genom a = createSampleGenom();
    neat::Genom b = createSampleGenom();

    auto c = neat::crossover(a, b, 1.0, 1.0);

    BOOST_CHECK_EQUAL(a.genes.size(), c.genes.size());
    for(std::size_t i = 0; i < a.genes.size(); ++i)
    {
        BOOST_CHECK_EQUAL(a.genes[i].dstNodeId, c.genes[i].dstNodeId);
        BOOST_CHECK_EQUAL(a.genes[i].innovationNumber, c.genes[i].innovationNumber);
        BOOST_CHECK_EQUAL(a.genes[i].srcNodeId, c.genes[i].srcNodeId);
        BOOST_CHECK(a.genes[i].weight == c.genes[i].weight || b.genes[i].weight == c.genes[i].weight);
        BOOST_CHECK_EQUAL(a.genes[i].enabled, c.genes[i].enabled);
    }
}

BOOST_FIXTURE_TEST_CASE( Test3, CrossoverTest ) 
{
    //Same innovation numbers, different weights, different tails
    neat::Genom a = createSampleGenom();
    a.genes.push_back({0, 8, true, 16, 0.55});

    neat::Genom b = createSampleGenom();
    b.genes.push_back({0, 9, true, 17, 0.44});
    b.genes.push_back({0, 10, true, 18, 0.33});

    {
        auto c = neat::crossover(a, b, 1.0, 0.0);

        BOOST_CHECK_EQUAL(a.genes.size(), c.genes.size());
        for(std::size_t i = 0; i < a.genes.size(); ++i)
        {
            BOOST_CHECK_EQUAL(a.genes[i].dstNodeId, c.genes[i].dstNodeId);
            BOOST_CHECK_EQUAL(a.genes[i].innovationNumber, c.genes[i].innovationNumber);
            BOOST_CHECK_EQUAL(a.genes[i].srcNodeId, c.genes[i].srcNodeId);
            if(a.genes[i].innovationNumber != 16)
            {
                BOOST_CHECK(a.genes[i].weight == c.genes[i].weight || b.genes[i].weight == c.genes[i].weight);
            }
            else
            {
                BOOST_CHECK(a.genes[i].weight == c.genes[i].weight);
            }
            BOOST_CHECK_EQUAL(a.genes[i].enabled, c.genes[i].enabled);
        }
    }
    {
        auto c = neat::crossover(a, b, 0.0, 1.0);

        BOOST_CHECK_EQUAL(b.genes.size(), c.genes.size());
        for(std::size_t i = 0; i < b.genes.size(); ++i)
        {
            BOOST_CHECK_EQUAL(b.genes[i].dstNodeId, c.genes[i].dstNodeId);
            BOOST_CHECK_EQUAL(b.genes[i].innovationNumber, c.genes[i].innovationNumber);
            BOOST_CHECK_EQUAL(b.genes[i].srcNodeId, c.genes[i].srcNodeId);
            if(b.genes[i].innovationNumber != 17 &&
                b.genes[i].innovationNumber != 18)
            {
                BOOST_CHECK(a.genes[i].weight == c.genes[i].weight || b.genes[i].weight == c.genes[i].weight);
            }
            else
            {
                BOOST_CHECK(b.genes[i].weight == c.genes[i].weight);
            }
            BOOST_CHECK_EQUAL(b.genes[i].enabled, c.genes[i].enabled);
        }
    }
}