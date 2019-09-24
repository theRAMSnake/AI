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
       return neat::Genom::createMinimal(3, 2);
   }
};

BOOST_FIXTURE_TEST_CASE( Test1, CrossoverTest ) 
{  
    //Same genom
    neat::Genom a = createSampleGenom();

    auto c = neat::Genom::crossover(a, a, 1.0, 1.0);

    BOOST_CHECK_EQUAL(a.length(), c.length());
    for(std::size_t i = 0; i < a.length(); ++i)
    {
        BOOST_CHECK_EQUAL(a[i].dstNodeId, c[i].dstNodeId);
        BOOST_CHECK_EQUAL(a[i].innovationNumber, c[i].innovationNumber);
        BOOST_CHECK_EQUAL(a[i].srcNodeId, c[i].srcNodeId);
        BOOST_CHECK_EQUAL(a[i].weight, c[i].weight);
        BOOST_CHECK_EQUAL(a[i].enabled, c[i].enabled);
    }
}

BOOST_FIXTURE_TEST_CASE( Test2, CrossoverTest ) 
{
    //Same innovation numbers, only weights different
    neat::Genom a = createSampleGenom();
    neat::Genom b = createSampleGenom();

    auto c = neat::Genom::crossover(a, b, 1.0, 1.0);

    BOOST_CHECK_EQUAL(a.length(), c.length());
    for(std::size_t i = 0; i < a.length(); ++i)
    {
        BOOST_CHECK_EQUAL(a[i].dstNodeId, c[i].dstNodeId);
        BOOST_CHECK_EQUAL(a[i].innovationNumber, c[i].innovationNumber);
        BOOST_CHECK_EQUAL(a[i].srcNodeId, c[i].srcNodeId);
        BOOST_CHECK(a[i].weight == c[i].weight || b[i].weight == c[i].weight);
        BOOST_CHECK_EQUAL(a[i].enabled, c[i].enabled);
    }
}

BOOST_FIXTURE_TEST_CASE( Test3, CrossoverTest ) 
{
    //Same innovation numbers, different weights, different tails
    neat::Genom a = createSampleGenom();
    a+= neat::Gene({0, 8, true, 16, 0.55});

    neat::Genom b = createSampleGenom();
    b+= neat::Gene({0, 9, true, 17, 0.44});
    b+= neat::Gene({0, 10, true, 18, 0.33});

    {
        auto c = neat::Genom::crossover(a, b, 1.0, 0.0);

        BOOST_CHECK_EQUAL(a.length(), c.length());
        for(std::size_t i = 0; i < a.length(); ++i)
        {
            BOOST_CHECK_EQUAL(a[i].dstNodeId, c[i].dstNodeId);
            BOOST_CHECK_EQUAL(a[i].innovationNumber, c[i].innovationNumber);
            BOOST_CHECK_EQUAL(a[i].srcNodeId, c[i].srcNodeId);
            if(a[i].innovationNumber != 16)
            {
                BOOST_CHECK(a[i].weight == c[i].weight || b[i].weight == c[i].weight);
            }
            else
            {
                BOOST_CHECK(a[i].weight == c[i].weight);
            }
            BOOST_CHECK_EQUAL(a[i].enabled, c[i].enabled);
        }
    }
    {
        auto c = neat::Genom::crossover(a, b, 0.0, 1.0);

        BOOST_CHECK_EQUAL(b.length(), c.length());
        for(std::size_t i = 0; i < b.length(); ++i)
        {
            BOOST_CHECK_EQUAL(b[i].dstNodeId, c[i].dstNodeId);
            BOOST_CHECK_EQUAL(b[i].innovationNumber, c[i].innovationNumber);
            BOOST_CHECK_EQUAL(b[i].srcNodeId, c[i].srcNodeId);
            if(b[i].innovationNumber != 17 &&
                b[i].innovationNumber != 18)
            {
                BOOST_CHECK(a[i].weight == c[i].weight || b[i].weight == c[i].weight);
            }
            else
            {
                BOOST_CHECK(b[i].weight == c[i].weight);
            }
            BOOST_CHECK_EQUAL(b[i].enabled, c[i].enabled);
        }
    }
}