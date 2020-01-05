#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "neat/neuro_net.hpp"

class NeuroNetTest
{
public:
   NeuroNetTest()
   {
   }

protected:
   neat::v2::Genom createSampleGenom()
   {
       return neat::v2::Genom::createMinimal(2, 1, mHistory, true);
   }

   neat::InnovationHistory mHistory;
};

BOOST_FIXTURE_TEST_CASE( TestSimpliest, NeuroNetTest ) 
{  
   neat::v2::Genom a = createSampleGenom();

   {
      neat::NeuroNet n(a);

      BOOST_CHECK_EQUAL(0, neat::activate(n, {0, 0})[0]);
   }
   
   a.setWeight(0, 0.5);
   a.setWeight(1, 0.5);

   {
      neat::NeuroNet n(a);

      BOOST_CHECK_EQUAL(10, neat::activate(n, {10, 10})[0]);
   }

   a.setWeight(0, 0.5);
   a.setWeight(1, 0.25);

   {
      neat::NeuroNet n(a);

      BOOST_CHECK_EQUAL(7.5, neat::activate(n, {10, 10})[0]);
   }
}

BOOST_FIXTURE_TEST_CASE( TestOneHiddenNode, NeuroNetTest ) 
{  
   neat::v2::Genom a = createSampleGenom();

   neat::v2::MutationConfig cfg;
   cfg.addNodeMutationChance = 1.0;
   a.mutate(cfg, mHistory);

   {
      a.setWeight(0, 0);
      a.setWeight(1, 0);
      a.setWeight(2, 0);

      neat::NeuroNet n(a);

      BOOST_CHECK_EQUAL(0, neat::activate(n, {0, 0})[0]);
   }
   {
      a.setWeight(0, 0.5);
      a.setWeight(1, 0.5);
      a.setWeight(2, 0.5);

      neat::NeuroNet n(a);

      BOOST_CHECK_EQUAL(5.5, neat::activate(n, {10, 10})[0]);
   }
   {
      a.setWeight(0, 0.5);
      a.setWeight(1, 0.5);
      a.setWeight(2, 0.5);

      a.connect(3, a.beginNodes(neat::v2::Genom::NodeType::Hidden)->id, mHistory, 1.0);

      neat::NeuroNet n(a);

      BOOST_CHECK_EQUAL(5.5, neat::activate(n, {10, 10})[0]);
   }
}

BOOST_FIXTURE_TEST_CASE( TestTriangleNode, NeuroNetTest ) 
{
   //Innovation numbers are irrelevant in this test
   neat::v2::Genom a = createSampleGenom();

   neat::v2::MutationConfig cfg;
   cfg.addNodeMutationChance = 1.0;
   a.mutate(cfg, mHistory);
   a.mutate(cfg, mHistory);
   a.mutate(cfg, mHistory);

   a.disconnectAll();

   auto iter = a.beginNodes(neat::v2::Genom::NodeType::Hidden);
   auto newNodeId1 = iter->id; ++iter;
   auto newNodeId2 = iter->id; ++iter;
   auto newNodeId3 = iter->id; ++iter;

   a.connect(0, newNodeId2, mHistory, 0.0);
   a.connect(1, newNodeId2, mHistory, 0.0);
   a.connect(1, newNodeId3, mHistory, 1.0);
   a.connect(2, newNodeId2, mHistory, 0.0);
   a.connect(2, newNodeId3, mHistory, 0.25);
   a.connect(newNodeId2, newNodeId1, mHistory, 0.5);
   a.connect(newNodeId1, newNodeId3, mHistory, 0.5);
   a.connect(newNodeId3, newNodeId2, mHistory, 0.5);
   a.connect(newNodeId1, 3, mHistory, 1.0);

   neat::NeuroNet n(a);
   BOOST_CHECK_EQUAL(0.5, neat::activate(n, {10, 10})[0]);
}