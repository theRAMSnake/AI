#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "neat/neuro_net.hpp"

class RNNTest
{
public:
   RNNTest()
   {
   }

protected:
   neat::v2::Genom createSampleGenom()
   {
       return neat::v2::Genom::createMinimal(2, 1, mHistory, true);
   }

   neat::InnovationHistory mHistory;
};

BOOST_FIXTURE_TEST_CASE( TestRNN, RNNTest ) 
{  
   neat::v2::Genom a = createSampleGenom();

   neat::v2::MutationConfig cfg;
   cfg.addNodeMutationChance = 1.0;
   a.mutate(cfg, mHistory);
   a.mutate(cfg, mHistory);

   auto iter = a.beginNodes(neat::v2::Genom::NodeType::Hidden);
   auto newNodeId1 = iter->id; ++iter;
   auto newNodeId2 = iter->id; ++iter;

   a.disconnectAll();

   a.connect(1, newNodeId1, mHistory); a.setWeight(a.getComplexity() - 1, 1.0);
   a.connect(2, newNodeId2, mHistory); a.setWeight(a.getComplexity() - 1, 1.0);
   a.connect(newNodeId2, newNodeId1, mHistory); a.setWeight(a.getComplexity() - 1, 1.0);
   a.connect(newNodeId1, 3, mHistory); a.setWeight(a.getComplexity() - 1, 1.0);
   a.connect(3, 3, mHistory); a.setWeight(a.getComplexity() - 1, 1.0);

   neat::NeuroNet n(a);
   BOOST_CHECK_EQUAL(0.92414181997875655, neat::activate(n, {0, 0})[0]);
   BOOST_CHECK_EQUAL(1.8482836399575131, neat::activate(n, {0, 0})[0]);
   BOOST_CHECK_EQUAL(2.7724254599362697, neat::activate(n, {0, 0})[0]);
}