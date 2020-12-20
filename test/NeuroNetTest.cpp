#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "neuroevolution/neuro_net2.hpp"
#include "neat/genom.hpp"

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
      auto n = neat::v2::createAnn2(a);

      BOOST_CHECK_EQUAL(0, neuroevolution::activate(*n, {0, 0})[0]);
   }
   
   a.setWeight(0, 0.5);
   a.setWeight(1, 0.5);

   {
      auto n = neat::v2::createAnn2(a);

      BOOST_CHECK_EQUAL(10, neuroevolution::activate(*n, {10, 10})[0]);
   }

   a.setWeight(0, 0.5);
   a.setWeight(1, 0.25);

   {
      auto n = neat::v2::createAnn2(a);

      BOOST_CHECK_EQUAL(7.5, neuroevolution::activate(*n, {10, 10})[0]);
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

      auto n = neat::v2::createAnn2(a);

      BOOST_CHECK_EQUAL(0, neuroevolution::activate(*n, {0, 0})[0]);
   }
   {
      a.setWeight(0, 0.5);
      a.setWeight(1, 0.5);
      a.setWeight(2, 0.5);

      auto n = neat::v2::createAnn2(a);

      BOOST_CHECK_EQUAL(5.5, neuroevolution::activate(*n, {10, 10})[0]);
   }
   {
      a.setWeight(0, 0.5);
      a.setWeight(1, 0.5);
      a.setWeight(2, 0.5);

      a.connect(3, a.beginNodes(neat::v2::Genom::NodeType::Hidden)->id, mHistory, 1.0);

      auto n = neat::v2::createAnn2(a);

      BOOST_CHECK_EQUAL(5.5, neuroevolution::activate(*n, {10, 10})[0]);
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

   auto n = neat::v2::createAnn2(a);
   BOOST_CHECK_EQUAL(0.5, neuroevolution::activate(*n, {10, 10})[0]);
}

/*BOOST_FIXTURE_TEST_CASE( TestSaveLoad, NeuroNetTest ) 
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

   auto n = neat::v2::createAnn(a);

   {
      std::ofstream f;

      f.open("nn.test.tmp", std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
      n->toBinaryStream(f);

      f.close();
   }

   {
      std::ifstream f;
      f.open("nn.test.tmp", std::ios_base::in | std::ios_base::binary);

      auto restored = neuroevolution::NeuroNet::fromBinaryStream(f);
      auto& r = *restored;

      auto a = neuroevolution::activate(r, {10, 10})[0];
      auto b = neuroevolution::activate(*n, {10, 10})[0];

      BOOST_CHECK_EQUAL(a, b);

      BOOST_CHECK_EQUAL(neuroevolution::activate(*restored, {5, 10})[0], 
                               neuroevolution::activate(*n, {5, 10})[0]);

      BOOST_CHECK_EQUAL(neuroevolution::activate(*restored, {3, 10})[0], neuroevolution::activate(*n, {3, 10})[0]);
      BOOST_CHECK_EQUAL(neuroevolution::activate(*restored, {10, -2})[0], neuroevolution::activate(*n, {10, -2})[0]);
   }
}*/