#define BOOST_TEST_DYN_LINK
#define TEST
#include <boost/test/unit_test.hpp>
#include "gacommon/neuro_net2.hpp"
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

      BOOST_CHECK_EQUAL(0, gacommon::activate(*n, {0, 0})[0]);
   }
   
   a.setWeight(0, 0.5);
   a.setWeight(1, 0.5);

   {
      auto n = neat::v2::createAnn2(a);

      BOOST_CHECK_EQUAL(10, gacommon::activate(*n, {10, 10})[0]);
   }

   a.setWeight(0, 0.5);
   a.setWeight(1, 0.25);

   {
      auto n = neat::v2::createAnn2(a);

      BOOST_CHECK_EQUAL(7.5, gacommon::activate(*n, {10, 10})[0]);
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

      BOOST_CHECK_EQUAL(0, gacommon::activate(*n, {0, 0})[0]);
   }
   {
      a.setWeight(0, 0.5);
      a.setWeight(1, 0.5);
      a.setWeight(2, 0.5);

      auto n = neat::v2::createAnn2(a);

      BOOST_CHECK_EQUAL(5.5, gacommon::activate(*n, {10, 10})[0]);
   }
   {
      a.setWeight(0, 0.5);
      a.setWeight(1, 0.5);
      a.setWeight(2, 0.5);

      a.connect(3, a.beginNodes(neat::v2::Genom::NodeType::Hidden)->id, mHistory, 1.0);

      auto n = neat::v2::createAnn2(a);

      BOOST_CHECK_EQUAL(5.5, gacommon::activate(*n, {10, 10})[0]);
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
   BOOST_CHECK_EQUAL(0.5, gacommon::activate(*n, {10, 10})[0]);
}

BOOST_FIXTURE_TEST_CASE( TestSaveLoad, NeuroNetTest )
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


   {
      std::ofstream f;

      f.open("nn.test.tmp", std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
      n->toBinaryStream(f);

      f.close();
   }

   {
      std::ifstream f;
      f.open("nn.test.tmp", std::ios_base::in | std::ios_base::binary);

      auto restored = gacommon::NeuroNet2::fromBinaryStream(f);
      BOOST_REQUIRE(restored);
      auto& r = *restored;

      auto a = gacommon::activate(r, {10, 10})[0];
      auto b = gacommon::activate(*n, {10, 10})[0];

      BOOST_CHECK_EQUAL(a, b);

      BOOST_CHECK_EQUAL(gacommon::activate(*restored, {5, 10})[0],
                               gacommon::activate(*n, {5, 10})[0]);

      BOOST_CHECK_EQUAL(gacommon::activate(*restored, {3, 10})[0], gacommon::activate(*n, {3, 10})[0]);
      BOOST_CHECK_EQUAL(gacommon::activate(*restored, {10, -2})[0], gacommon::activate(*n, {10, -2})[0]);
   }
}

BOOST_FIXTURE_TEST_CASE( TestSaveLoad2, NeuroNetTest )
{
    const std::vector<gacommon::NodeId> inputNodes = {0, 1, 2, 3, 4};
    const std::vector<gacommon::NodeId> outputNodes = {5, 6};
    const std::vector<gacommon::NeuroNet2::HiddenNodeDef> hiddenNodes = {
        {7, ActivationFunctionType::SIGMOID, 0},
        {8, ActivationFunctionType::IDENTITY, 0.5}
    };
    const std::vector<gacommon::NeuroNet2::ConnectionDef> connections = {
        {0, 7, 1},
        {1, 7, -1},
        {0, 8, -1},
        {7, 8, 1},
        {8, 6, 0},
    };

    gacommon::NeuroNet2 nn(inputNodes, outputNodes, hiddenNodes, connections);
    {
       std::ofstream f;

       f.open("nn.test.tmp", std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
       nn.toBinaryStream(f);

       f.close();
    }
    {
       std::ifstream f;
       f.open("nn.test.tmp", std::ios_base::in | std::ios_base::binary);

       auto restored = gacommon::NeuroNet2::fromBinaryStream(f);
       BOOST_REQUIRE(restored);

       BOOST_CHECK_EQUAL(nn.mNodes.size(), restored->mNodes.size());
       BOOST_CHECK_EQUAL(nn.mInputNodes.size(), restored->mInputNodes.size());
       BOOST_CHECK_EQUAL(nn.mOutputNodes.size(), restored->mOutputNodes.size());
       BOOST_CHECK_EQUAL(nn.mHiddenNodes.size(), restored->mHiddenNodes.size());
       BOOST_CHECK_EQUAL(nn.mValues.size(), restored->mValues.size());

       for(std::size_t i = 0; i < nn.mInputNodes.size(); ++i)
       {
           BOOST_CHECK_EQUAL(nn.mInputNodes[i]->id, restored->mInputNodes[i]->id);
       }
       for(std::size_t i = 0; i < nn.mOutputNodes.size(); ++i)
       {
           BOOST_CHECK_EQUAL(nn.mOutputNodes[i]->id, restored->mOutputNodes[i]->id);
       }
       for(std::size_t i = 0; i < nn.mHiddenNodes.size(); ++i)
       {
           BOOST_CHECK_EQUAL(nn.mHiddenNodes[i]->id, restored->mHiddenNodes[i]->id);
       }
       for(std::size_t i = 0; i < nn.mValues.size(); ++i)
       {
           BOOST_CHECK_EQUAL(nn.mValues[i], restored->mValues[i]);
       }
       for(std::size_t i = 0; i < nn.mNodes.size(); ++i)
       {
           BOOST_CHECK_EQUAL(nn.mNodes[i].id, restored->mNodes[i].id);
           BOOST_CHECK_EQUAL(nn.mNodes[i].bias, restored->mNodes[i].bias);
           BOOST_CHECK_EQUAL(static_cast<int>(nn.mNodes[i].accType), static_cast<int>(restored->mNodes[i].accType));
           if(nn.mNodes[i].func != nullptr)
           {
               BOOST_CHECK_EQUAL(nn.mNodes[i].func(0.5), restored->mNodes[i].func(0.5));
           }
           BOOST_CHECK_EQUAL(nn.mNodes[i].inputs.size(), restored->mNodes[i].inputs.size());
           for(std::size_t j = 0; j < nn.mNodes[i].inputs.size(); ++j)
           {
               BOOST_CHECK_EQUAL(nn.mNodes[i].inputs[j].first, restored->mNodes[i].inputs[j].first);
               BOOST_CHECK_EQUAL(nn.mNodes[i].inputs[j].second, restored->mNodes[i].inputs[j].second);
           }
       }
    }
}

BOOST_FIXTURE_TEST_CASE( TestNNAgent, NeuroNetTest )
{
   std::vector<gacommon::IOElement> inputs = {
       gacommon::ValueIO{0},
       gacommon::ValueIO{1},
       gacommon::ChoiceIO{3, 1},
       gacommon::BitmapIO{3, 3, {1, 1, 1, 2, 2, 2, 3, 3, 3}},
       gacommon::ValueIO{5},
   };
   std::vector<gacommon::IOElement> outputs = {
       gacommon::ValueIO{0},
       gacommon::ValueIO{0},
       gacommon::ChoiceIO{3, 0},
       gacommon::BitmapIO{3, 3, {0, 0, 0, 0, 0, 0, 0, 0, 0}},
       gacommon::ValueIO{0},
   };

   //Create an nn with a full translation
   const std::size_t totalInputs = 15;
   const std::size_t totalOutputs = 15;

   const std::vector<gacommon::NodeId> inputNodes = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
   const std::vector<gacommon::NodeId> outputNodes = {15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29};
   std::vector<gacommon::NeuroNet2::ConnectionDef> connections;
   for(std::size_t i = 0; i < totalOutputs; ++i)
   {
       connections.push_back({inputNodes[i], outputNodes[i], 1});
   }
   auto nn = std::make_unique<gacommon::NeuroNet2>(inputNodes, outputNodes, std::vector<gacommon::NeuroNet2::HiddenNodeDef>{}, connections);
   gacommon::NNAgent agent(totalInputs, totalOutputs, std::move(nn));

   agent.run(inputs, outputs);

   BOOST_CHECK_EQUAL(0, std::get<gacommon::ValueIO>(outputs[0]).value);
   BOOST_CHECK_EQUAL(1, std::get<gacommon::ValueIO>(outputs[1]).value);
   BOOST_CHECK_EQUAL(5, std::get<gacommon::ValueIO>(outputs[4]).value);
   BOOST_CHECK_EQUAL(1, std::get<gacommon::ChoiceIO>(outputs[2]).selection);
   for(std::size_t i = 0; i < 9; ++i)
   {
       BOOST_CHECK_EQUAL(std::get<gacommon::BitmapIO>(inputs[3]).map[i], std::get<gacommon::BitmapIO>(outputs[3]).map[i]);
   }
}
