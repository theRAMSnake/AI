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
       return neat::Genom::createMinimal(3, 2);
   }

   const neat::InnovationNumber INNITIAL_INNOVATION_NUMBER = 3 * 2;
};

BOOST_FIXTURE_TEST_CASE( AddNodeMutation, MutationTest ) 
{  
   auto g = createSampleGenom();

   neat::InnovationNumber innovationNumber = INNITIAL_INNOVATION_NUMBER;
   neat::mutateAddNode(g, innovationNumber);

   neat::NodeId newNodeId = 5;
   auto outgoingC = std::find_if(g.begin(), g.end(), [&](auto x){return x.srcNodeId == newNodeId;});
   auto ingoingC = std::find_if(g.begin(), g.end(), [&](auto x){return x.dstNodeId == newNodeId;});
   BOOST_CHECK(outgoingC != g.end());
   BOOST_CHECK(ingoingC != g.end());

   auto disabledCon = std::find_if(g.begin(), g.end(), [&](auto x){return x.srcNodeId == ingoingC->srcNodeId && x.dstNodeId == outgoingC->dstNodeId;});
   BOOST_CHECK(disabledCon != g.end());
   BOOST_CHECK(!disabledCon->enabled);

   BOOST_CHECK(outgoingC->enabled);
   BOOST_CHECK(ingoingC->enabled);
   BOOST_CHECK(ingoingC->innovationNumber == INNITIAL_INNOVATION_NUMBER + 1);
   BOOST_CHECK(outgoingC->innovationNumber == INNITIAL_INNOVATION_NUMBER + 2);
   BOOST_CHECK(innovationNumber == INNITIAL_INNOVATION_NUMBER + 2);

   BOOST_CHECK_EQUAL(1.0, ingoingC->weight);
   BOOST_CHECK_EQUAL(disabledCon->weight, outgoingC->weight);
}

BOOST_FIXTURE_TEST_CASE( AddConnectionMutation, MutationTest ) 
{
   auto g = createSampleGenom();

   neat::InnovationNumber innovationNumber = INNITIAL_INNOVATION_NUMBER;
   neat::mutateAddNode(g, innovationNumber);

   neat::NodeId newNodeId = 5;
   auto outgoingC = std::find_if(g.begin(), g.end(), [&](auto x){return x.srcNodeId == newNodeId;});
   auto ingoingC = std::find_if(g.begin(), g.end(), [&](auto x){return x.dstNodeId == newNodeId;});
   BOOST_CHECK(outgoingC != g.end());
   BOOST_CHECK(ingoingC != g.end());

   neat::mutateAddConnection(g, innovationNumber);
   BOOST_CHECK(innovationNumber == INNITIAL_INNOVATION_NUMBER + 3);

   //It is only possible to connect from or to our new node
   //The new connection will be at the end of the genom
   auto newConnection = g[g.length() - 1];
   BOOST_CHECK(newConnection.enabled);
   BOOST_CHECK_EQUAL(INNITIAL_INNOVATION_NUMBER + 3, newConnection.innovationNumber);
   printf("%d -> %d: %f", newConnection.srcNodeId, newConnection.dstNodeId, newConnection.weight);
   BOOST_CHECK(newConnection.srcNodeId == newNodeId || newConnection.dstNodeId == newNodeId);
   BOOST_CHECK_EQUAL(9, g.length());
}