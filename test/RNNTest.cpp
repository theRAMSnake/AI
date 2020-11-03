#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "neuroevolution/neuro_net.hpp"
#include "neat/genom.hpp"

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
   std::cout << newNodeId1;
   auto newNodeId2 = iter->id; ++iter;

   a.disconnectAll();

   a.connect(1, newNodeId1, mHistory, 1.0);
   a.connect(2, newNodeId2, mHistory, 1.0);
   a.connect(newNodeId2, newNodeId1, mHistory, 1.0);
   a.connect(newNodeId1, 3, mHistory, 1.0);
   a.connect(3, 3, mHistory, 1.0);

   auto n = neat::v2::createAnn2(a);
   BOOST_CHECK_EQUAL(0.92414181997875655, neuroevolution::activate(*n, {0, 0})[0]);
   BOOST_CHECK_EQUAL(1.8482836399575131, neuroevolution::activate(*n, {0, 0})[0]);
   BOOST_CHECK_EQUAL(2.7724254599362697, neuroevolution::activate(*n, {0, 0})[0]);
}

BOOST_FIXTURE_TEST_CASE( TestRNN1, RNNTest )
{
   neuroevolution::NeuroNet2::Node n0 = {
      0, 0.1, 0.0, -1,
      {
         { 3, 0.12 }
      },
      &sigmoid, ActivationFunctionType::SIGMOID
   };
   neuroevolution::NeuroNet2::Node n1 = {
      0, 0.1, 0.0, -1,
      {
         { 0, 0.1 },
         { 2, 0.2 }
      },
      &sigmoid, ActivationFunctionType::SIGMOID
   };
   neuroevolution::NeuroNet2::Node n2 = {
      0, 0.1, 0.0, -1,
      {
         { 0, 0.05 }
      },
      &sigmoid, ActivationFunctionType::SIGMOID
   };
   neuroevolution::NeuroNet2::Node n3 = {
      0, 0.1, 0.0, -1,
      {
         { 1, 0.07 }
      },
      &sigmoid, ActivationFunctionType::SIGMOID
   };
   neuroevolution::NeuroNet2::Node n4 = {
      0, 0.1, 0.0, -1,
      {
         { 1, 0.16 },
         { 2, 0.21 }
      },
      &sigmoid, ActivationFunctionType::SIGMOID
   };

   std::vector<neuroevolution::NeuroNet2::Node> nodes = { n0, n1, n2, n3, n4 };
   std::size_t numConnections = 7;


   neuroevolution::NeuroNet2::Matrix matrix(nodes, numConnections);

   std::vector<std::uint16_t> expectedColInd = { 0, 1, 3, 4, 5, 7 };
   for(int i = 0; i < 6; i++)
   {
      BOOST_CHECK_EQUAL(matrix.colInd[i], expectedColInd[i]);
   }
   BOOST_CHECK_EQUAL(matrix.colInd.size(), 6);

   std::vector<std::uint16_t> expectedRowInd = { 3, 0, 2, 0, 1, 1, 2 };
   for(int i = 0; i < 7; i++)
   {
      BOOST_CHECK_EQUAL(matrix.rowInd[i], expectedRowInd[i]);
   }
   BOOST_CHECK_EQUAL(matrix.rowInd.size(), 7);
}