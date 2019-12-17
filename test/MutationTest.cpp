#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "neat/genom.hpp"

class MutationTest
{
public:
   MutationTest()
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

      neat::Genom::setConfig(c);
   }

protected:
   neat::Genom createSampleGenom()
   {
       return neat::Genom::createMinimal(3, 2, mHistory, true);
   }

   neat::InnovationHistory mHistory;
};

BOOST_FIXTURE_TEST_CASE( AddNodeMutation, MutationTest ) 
{  
   auto g = createSampleGenom();

   neat::mutateAddNode(g, mHistory);

   neat::NodeId newNodeId = 6;
   auto outgoingC = std::find_if(g.begin(), g.end(), [&](auto x){return x.srcNodeId == newNodeId;});
   auto ingoingC = std::find_if(g.begin(), g.end(), [&](auto x){return x.dstNodeId == newNodeId;});
   BOOST_CHECK(outgoingC != g.end());
   BOOST_CHECK(ingoingC != g.end());

   auto disabledCon = std::find_if(g.begin(), g.end(), [&](auto x){return x.srcNodeId == ingoingC->srcNodeId && x.dstNodeId == outgoingC->dstNodeId;});
   BOOST_CHECK(disabledCon != g.end());
   BOOST_CHECK(!disabledCon->enabled);

   BOOST_CHECK(outgoingC->enabled);
   BOOST_CHECK(ingoingC->enabled);
   BOOST_CHECK(ingoingC->innovationNumber == 6);
   BOOST_CHECK(outgoingC->innovationNumber == 7);

   BOOST_CHECK_EQUAL(1.0, ingoingC->weight);
   BOOST_CHECK_EQUAL(disabledCon->weight, outgoingC->weight);
}

BOOST_FIXTURE_TEST_CASE( AddConnectionMutation, MutationTest ) 
{
   auto g = createSampleGenom();

   neat::mutateAddNode(g, mHistory);

   neat::NodeId newNodeId = 6;
   auto outgoingC = std::find_if(g.begin(), g.end(), [&](auto x){return x.srcNodeId == newNodeId;});
   auto ingoingC = std::find_if(g.begin(), g.end(), [&](auto x){return x.dstNodeId == newNodeId;});
   BOOST_CHECK(outgoingC != g.end());
   BOOST_CHECK(ingoingC != g.end());

   neat::mutateAddConnection(g, mHistory);

   //It is only possible to connect from or to our new node
   //The new connection will be at the end of the genom
   //Another option here is bias->output connection
   auto newConnection = g[g.length() - 1];
   BOOST_CHECK(newConnection.enabled);
   BOOST_CHECK_EQUAL(8, newConnection.innovationNumber);
   BOOST_CHECK(newConnection.srcNodeId == newNodeId || newConnection.dstNodeId == newNodeId
      || (newConnection.srcNodeId == 0 && g.isOutputNode(newConnection.dstNodeId)));
   BOOST_CHECK_EQUAL(9, g.length());
}

BOOST_FIXTURE_TEST_CASE( RemoveConnectionMutation, MutationTest ) 
{
   auto g = createSampleGenom();

   neat::mutateRemoveConnection(g);

   auto disabledCon = std::find_if(g.begin(), g.end(), [&](auto x){return !x.enabled;});
   BOOST_CHECK(disabledCon != g.end());
}

BOOST_FIXTURE_TEST_CASE( AddConnectionMutation_EnableAgain, MutationTest ) 
{
   auto g = createSampleGenom();
   g.begin()->enabled = false;

   for(int i = 0; i < 20; ++i)
   {
      neat::mutateAddConnection(g, mHistory);
   }
   
   BOOST_CHECK(g.begin()->enabled);
}

static void printGenom(const neat::Genom& g)
{
   std::cout << "         (";

   for(auto& x : g)
   {
      if(x.enabled)
      {
         std::cout << x.srcNodeId << "->" << x.dstNodeId << " ";
      }
   }

   std::cout << ")" << std::endl;
}

BOOST_FIXTURE_TEST_CASE( RemoveNodeMutation, MutationTest ) 
{
   //No nodes
   {
      auto g = createSampleGenom();
      neat::mutateRemoveNode(g, mHistory);

      BOOST_CHECK_EQUAL(6, g.length());
   }

   //1 node, 1-1 connection
   {
      auto g = createSampleGenom();
      neat::mutateAddNode(g, mHistory);
      neat::NodeId newNodeId = 6;
      
      auto disabledCon = std::find_if(g.begin(), g.end(), [&](auto x){return !x.enabled;});
      auto src = disabledCon->srcNodeId;
      auto dst = disabledCon->dstNodeId;

      neat::mutateRemoveNode(g, mHistory);

      BOOST_CHECK(!std::find_if(g.begin(), g.end(), [&](auto x){return x.srcNodeId == newNodeId;})->enabled);
      BOOST_CHECK(!std::find_if(g.begin(), g.end(), [&](auto x){return x.dstNodeId == newNodeId;})->enabled);

      BOOST_CHECK(std::find_if(g.begin(), g.end(), [&](auto x){return x.dstNodeId == dst && x.srcNodeId == src;})->enabled);
   }

   //5 nodes
   {
      auto g = createSampleGenom();
      for(int i = 0; i < 5; ++i)
      {
         neat::mutateAddNode(g, mHistory);
      }

      printGenom(g);
      
      for(int i = 0; i < 5; ++i)
      {
         neat::mutateRemoveNode(g, mHistory);
      }

      BOOST_CHECK_EQUAL(0, g.getNumConnectedHiddenNodes());
   }

   //1-n connections
   {
      auto g = neat::Genom::createMinimal(3, 2, mHistory, false);
      
      //Create node with all(3) sources connected to one output
      g += {1, 6, true, 0, 1.0};
      g += {2, 6, true, 0, 1.0};
      g += {3, 6, true, 0, 1.0};
      g += {6, 4, true, 0, 5.0};

      neat::mutateRemoveNode(g, mHistory);
      
      BOOST_CHECK(!g.isConnected(1, 6));
      BOOST_CHECK(!g.isConnected(2, 6));
      BOOST_CHECK(!g.isConnected(3, 6));
      BOOST_CHECK(!g.isConnected(6, 1));

      //After removal all src nodes are connected to those output with avg weight
      BOOST_CHECK(g.isConnected(1, 4));
      BOOST_CHECK(g.isConnected(2, 4));
      BOOST_CHECK(g.isConnected(3, 4));
   }

   //n-n connections
   {
      auto g = neat::Genom::createMinimal(3, 2, mHistory, false);
      
      //Create node with all(3) sources connected to one output
      g += {1, 6, true, 0, 1.0};
      g += {2, 6, true, 0, 1.0};
      g += {3, 6, true, 0, 1.0};
      g += {6, 4, true, 0, 5.0};
      g += {6, 5, true, 0, 5.0};

      auto gCopy = g;
      neat::mutateRemoveNode(gCopy, mHistory);

      BOOST_CHECK_EQUAL(0, neat::Genom::calculateDivergence(g, gCopy));
   }
}