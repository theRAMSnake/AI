#define BOOST_TEST_MODULE a
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#define TEST
#include "snakega/snakega.hpp"
#include "neuroevolution/rng.hpp"

class SnakeGATest
{
public:
   SnakeGATest()
   {

   }

protected:
   //2. Decoder
   //5. Mutations
   //6. Test convergence
   void populateGenomWithNodes(snakega::Genom& g)
   {
        g.mDefs.push_back({1, {{0, ActivationFunctionType::IDENTITY},
          {1, ActivationFunctionType::IDENTITY},
          {2, ActivationFunctionType::IDENTITY},
          {3, ActivationFunctionType::IDENTITY}
          }});

        g.mDefs.push_back({2, {{0, ActivationFunctionType::IDENTITY},
          {1, ActivationFunctionType::IDENTITY},
          {2, ActivationFunctionType::IDENTITY},
          {3, ActivationFunctionType::IDENTITY}
          }});

        g.mDefs.push_back({3});

        g.mBlocks.push_back({1, 0, 1});
        g.mBlocks.push_back({2, 0, 1});
        g.mBlocks.push_back({3, 0, 3});
        g.mBlocks.push_back({4, 0, 3});
        g.mBlocks.push_back({5, 0, 2});
        g.mBlocks.push_back({6, 0, 2});

        //This should produce 4 * 4 = 16 nodes
   }

   void populateGenomWithConnections(snakega::Genom& g)
   {
        g.mDefs.push_back({1, {{0, ActivationFunctionType::IDENTITY},
          {1, ActivationFunctionType::IDENTITY}}, 
          {{0, 1, 0.5}, {1, 0, 0.5}}});

        g.mDefs.push_back({2, {{0, ActivationFunctionType::IDENTITY},
          {1, ActivationFunctionType::IDENTITY}},
          {{0, 1, 0.5}, {1, 0, 0.5}}});

        g.mDefs.push_back({3});

        g.mBlocks.push_back({1, 0, 1, {
            {snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Input, 0, 0), snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 1, 0), 0.5}, 
            {snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Input, 0, 0), snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 1, 1), 0.5}, 
            {snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 1, 1), snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Output, 0, 1), 0.5}}});

        g.mBlocks.push_back({2, 0, 1});
        g.mBlocks.push_back({3, 0, 3});
        g.mBlocks.push_back({4, 0, 3});
        g.mBlocks.push_back({5, 0, 2, {
            {snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Input, 0, 0), snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 5, 0), 0.5}, 
            {snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Input, 0, 0), snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 1, 1), 0.5}, 
            {snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 5, 1), snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Output, 0, 1), 0.5}}});
        g.mBlocks.push_back({6, 0, 2});
   }
};

class TestFitnessEvaluator : public neuroevolution::IFitnessEvaluator
{
public:
    neuroevolution::Fitness evaluate(neuroevolution::IAgent& agent)
    {
        return Rng::genReal();
    }
};

BOOST_FIXTURE_TEST_CASE( TenGenerationsStress, SnakeGATest ) 
{
    snakega::Config cfg{50, 3, 3, 50, 0.25, 3};
    neuroevolution::DomainGeometry dg {{1, 1, 1}, {{1, 1}, {1, 1}, {1, 1}, {1, 1}}, {{1, 1}, {1, 1}, {1, 1}, {1, 1}}};
    TestFitnessEvaluator eval;

    snakega::Algorithm alg(cfg, dg, eval);

    for(int i = 0; i < 10; ++i)
    {
        alg.step();
    }
}

BOOST_FIXTURE_TEST_CASE( NodesIteration, SnakeGATest ) 
{
    snakega::Genom sample = snakega::Genom::createMinimal(4, 4);
    BOOST_CHECK_EQUAL(0, std::distance(sample.beginNodes(), sample.endNodes()));

    populateGenomWithNodes(sample);

    BOOST_CHECK_EQUAL(16, std::distance(sample.beginNodes(), sample.endNodes()));

    auto iter = sample.beginNodes();
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 1, 0) == iter->id); ++iter;
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 1, 1) == iter->id); ++iter;
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 1, 2) == iter->id); ++iter;
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 1, 3) == iter->id); ++iter;
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 2, 0) == iter->id); ++iter;
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 2, 1) == iter->id); ++iter;
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 2, 2) == iter->id); ++iter;
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 2, 3) == iter->id); ++iter;
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 5, 0) == iter->id); ++iter;
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 5, 1) == iter->id); ++iter;
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 5, 2) == iter->id); ++iter;
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 5, 3) == iter->id); ++iter;
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 6, 0) == iter->id); ++iter;
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 6, 1) == iter->id); ++iter;
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 6, 2) == iter->id); ++iter;
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 6, 3) == iter->id); ++iter;
    BOOST_CHECK(iter == sample.endNodes());
}

BOOST_FIXTURE_TEST_CASE( ConnectionsIteration, SnakeGATest ) 
{
    snakega::Genom sample(4, 4);
    BOOST_CHECK_EQUAL(0, std::distance(sample.beginConnections(), sample.endConnections()));

    snakega::Genom minimal = snakega::Genom::createMinimal(4, 4);
    BOOST_CHECK_EQUAL(16, std::distance(minimal.beginConnections(), minimal.endConnections()));

    populateGenomWithConnections(minimal);

    BOOST_CHECK_EQUAL(16 + 14, std::distance(minimal.beginConnections(), minimal.endConnections()));

    auto iter = minimal.beginConnections();
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Input, 0, 0) == iter->srcNodeId); 
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Output, 0, 0) == iter->dstNodeId); 
    ++iter;

    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Input, 0, 0) == iter->srcNodeId); 
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Output, 0, 1) == iter->dstNodeId); 
    ++iter;

    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Input, 0, 0) == iter->srcNodeId); 
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Output, 0, 2) == iter->dstNodeId); 
    ++iter;

    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Input, 0, 0) == iter->srcNodeId); 
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Output, 0, 3) == iter->dstNodeId); 
    ++iter;

    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Input, 0, 1) == iter->srcNodeId); 
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Output, 0, 0) == iter->dstNodeId); 
    ++iter;

    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Input, 0, 1) == iter->srcNodeId); 
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Output, 0, 1) == iter->dstNodeId); 
    ++iter;

    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Input, 0, 1) == iter->srcNodeId); 
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Output, 0, 2) == iter->dstNodeId); 
    ++iter;

    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Input, 0, 1) == iter->srcNodeId); 
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Output, 0, 3) == iter->dstNodeId); 
    ++iter;

    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Input, 0, 2) == iter->srcNodeId); 
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Output, 0, 0) == iter->dstNodeId); 
    ++iter;

    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Input, 0, 2) == iter->srcNodeId); 
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Output, 0, 1) == iter->dstNodeId); 
    ++iter;

    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Input, 0, 2) == iter->srcNodeId); 
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Output, 0, 2) == iter->dstNodeId); 
    ++iter;

    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Input, 0, 2) == iter->srcNodeId); 
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Output, 0, 3) == iter->dstNodeId); 
    ++iter;

    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Input, 0, 3) == iter->srcNodeId); 
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Output, 0, 0) == iter->dstNodeId); 
    ++iter;

    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Input, 0, 3) == iter->srcNodeId); 
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Output, 0, 1) == iter->dstNodeId); 
    ++iter;

    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Input, 0, 3) == iter->srcNodeId); 
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Output, 0, 2) == iter->dstNodeId); 
    ++iter;

    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Input, 0, 3) == iter->srcNodeId); 
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Output, 0, 3) == iter->dstNodeId); 
    ++iter;

    //Block 1

    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 1, 0) == iter->srcNodeId); 
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 1, 1) == iter->dstNodeId); 
    ++iter;

    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 1, 1) == iter->srcNodeId); 
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 1, 0) == iter->dstNodeId); 
    ++iter;

    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Input, 0, 0) == iter->srcNodeId); 
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 1, 0) == iter->dstNodeId); 
    ++iter;

    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Input, 0, 0) == iter->srcNodeId); 
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 1, 1) == iter->dstNodeId); 
    ++iter;

    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 1, 1) == iter->srcNodeId); 
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Output, 0, 1) == iter->dstNodeId); 
    ++iter;

    //Block 2

    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 2, 0) == iter->srcNodeId); 
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 2, 1) == iter->dstNodeId); 
    ++iter;

    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 2, 1) == iter->srcNodeId);
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 2, 0) == iter->dstNodeId); 
    ++iter;

    // Block 5

    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 5, 0) == iter->srcNodeId); 
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 5, 1) == iter->dstNodeId); 
    ++iter;

    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 5, 1) == iter->srcNodeId); 
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 5, 0) == iter->dstNodeId); 
    ++iter;

    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Input, 0, 0) == iter->srcNodeId); 
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 5, 0) == iter->dstNodeId); 
    ++iter;

    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Input, 0, 0) == iter->srcNodeId); 
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 1, 1) == iter->dstNodeId); 
    ++iter;

    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 5, 1) == iter->srcNodeId); 
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Output, 0, 1) == iter->dstNodeId); 
    ++iter;

    // Block 6

    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 6, 0) == iter->srcNodeId); 
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 6, 1) == iter->dstNodeId); 
    ++iter;

    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 6, 1) == iter->srcNodeId);
    BOOST_CHECK(snakega::Genom::GlobalNodeId(snakega::Genom::GlobalNodeType::Local, 6, 0) == iter->dstNodeId); 
    ++iter;
}