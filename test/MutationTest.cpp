#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "neat/genom.hpp"

class MutationTest
{
public:
   MutationTest()
   {
   }

protected:
   neat::v2::Genom createSampleGenom()
   {
       return neat::v2::Genom::createMinimal(3, 2, mHistory, true);
   }

   neat::InnovationHistory mHistory;
};

BOOST_FIXTURE_TEST_CASE( AddNodeMutation, MutationTest ) 
{  
   auto g = createSampleGenom();

   neat::v2::MutationConfig cfg;
   cfg.addNodeMutationChance = 1.0;
   g.mutate(cfg, mHistory);

   BOOST_CHECK_EQUAL(1, g.getNodeCount(neat::v2::Genom::NodeType::Hidden));

   neat::NodeId newNodeId = g.beginNodes(neat::v2::Genom::NodeType::Hidden)->id;
   auto outgoingC = std::find_if(g.begin(), g.end(), [&](auto x){return x.srcNodeId == newNodeId;});
   auto ingoingC = std::find_if(g.begin(), g.end(), [&](auto x){return x.dstNodeId == newNodeId;});
   BOOST_CHECK(outgoingC != g.end());
   BOOST_CHECK(ingoingC != g.end());

   BOOST_CHECK(ingoingC->innovationNumber == 6);
   BOOST_CHECK(outgoingC->innovationNumber == 7);

   BOOST_CHECK_EQUAL(1.0, ingoingC->weight);
}

BOOST_FIXTURE_TEST_CASE( AddConnectionMutation, MutationTest ) 
{
   auto g = createSampleGenom();

   neat::v2::MutationConfig cfg;
   cfg.addNodeMutationChance = 1.0;
   g.mutate(cfg, mHistory);

   neat::NodeId newNodeId = g.beginNodes(neat::v2::Genom::NodeType::Hidden)->id;
   auto outgoingC = std::find_if(g.begin(), g.end(), [&](auto x){return x.srcNodeId == newNodeId;});
   auto ingoingC = std::find_if(g.begin(), g.end(), [&](auto x){return x.dstNodeId == newNodeId;});
   BOOST_CHECK(outgoingC != g.end());
   BOOST_CHECK(ingoingC != g.end());

   cfg.addNodeMutationChance = 0.0;
   cfg.addConnectionMutationChance = 1.0;
   g.mutate(cfg, mHistory);

   //It is only possible to connect from or to our new node
   //The new connection will be at the end of the genom
   //Another options here are bias->output connection or recurrent
   auto newConnection = g[g.getComplexity() - 1];
   BOOST_CHECK_EQUAL(8, newConnection.innovationNumber);

   std::cout << std::endl << newConnection.srcNodeId << " " << newConnection.dstNodeId << std::endl;

   BOOST_CHECK(newConnection.srcNodeId == newNodeId || newConnection.dstNodeId == newNodeId
      || (newConnection.dstNodeId == 4 || newConnection.dstNodeId == 5));

   BOOST_CHECK_EQUAL(8, g.getComplexity());
}

BOOST_FIXTURE_TEST_CASE( RemoveConnectionMutation, MutationTest ) 
{
   auto g = createSampleGenom();

   auto gSize = g.getComplexity();

   neat::v2::MutationConfig cfg;
   cfg.removeConnectionMutationChance = 1.0;
   g.mutate(cfg, mHistory);

   BOOST_CHECK_EQUAL(gSize - 1, g.getComplexity());   
}

BOOST_FIXTURE_TEST_CASE( RemoveNodeMutation, MutationTest ) 
{
   //No nodes
   {
      auto g = createSampleGenom();
      neat::v2::MutationConfig cfg;
      cfg.removeNodeMutationChance = 1.0;
      g.mutate(cfg, mHistory);

      BOOST_CHECK_EQUAL(6, g.getComplexity());
   }

   //1 node, 1-1 connection
   {
      auto g = createSampleGenom();
      {
         neat::v2::MutationConfig cfg;
         cfg.addNodeMutationChance = 1.0;
         g.mutate(cfg, mHistory);

         BOOST_CHECK_EQUAL(1, g.getNodeCount(neat::v2::Genom::NodeType::Hidden));
      }
      neat::NodeId newNodeId = g.beginNodes(neat::v2::Genom::NodeType::Hidden)->id;
      {
         neat::v2::MutationConfig cfg;
         cfg.removeNodeMutationChance = 1.0;
         g.mutate(cfg, mHistory);

         BOOST_CHECK_EQUAL(0, g.getNodeCount(neat::v2::Genom::NodeType::Hidden));
      }

      BOOST_CHECK(std::find_if(g.begin(), g.end(), [&](auto x){return x.srcNodeId == newNodeId;}) == g.end());
      BOOST_CHECK(std::find_if(g.begin(), g.end(), [&](auto x){return x.dstNodeId == newNodeId;}) == g.end());
   }

   //5 nodes
   {
      auto g = createSampleGenom();
      for(int i = 0; i < 5; ++i)
      {
         neat::v2::MutationConfig cfg;
         cfg.addNodeMutationChance = 1.0;
         g.mutate(cfg, mHistory);
      }

      BOOST_CHECK_EQUAL(5, g.getNodeCount(neat::v2::Genom::NodeType::Hidden));

      for(int i = 0; i < 5; ++i)
      {
         neat::v2::MutationConfig cfg;
         cfg.removeNodeMutationChance = 1.0;
         g.mutate(cfg, mHistory);
      }

      BOOST_CHECK_EQUAL(0, g.getNodeCount(neat::v2::Genom::NodeType::Hidden));
   }
}