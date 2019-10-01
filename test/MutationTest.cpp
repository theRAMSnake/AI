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
   neat::Genom createSampleGenom()
   {
       return neat::Genom::createMinimal(3, 2, mHistory);
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