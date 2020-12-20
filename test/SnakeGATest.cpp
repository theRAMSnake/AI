#define BOOST_TEST_MODULE a
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#define TEST
#include "snakega/snakega.hpp"
#include "snakega/decoder.hpp"
#include "neuroevolution/rng.hpp"

class SnakeGATest
{
public:
   SnakeGATest()
   {

   }

protected:
   //5. Mutations
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
    snakega::Config cfg{10, 3, 3, 10, 0.25, 1};
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

BOOST_FIXTURE_TEST_CASE( Decode, SnakeGATest ) 
{
    snakega::Genom minimal = snakega::Genom::createMinimal(4, 4);
    for(auto& c : minimal.mBlocks[0].externalConnections)
    {
        c.weight = 1.0;
    }

    {
        auto nn = snakega::GenomDecoder::decode(minimal);    
        auto other = minimal;
        auto nn2 = snakega::GenomDecoder::decode(minimal);    
    }
    

    populateGenomWithConnections(minimal);

    auto nn = snakega::GenomDecoder::decode(minimal);

    std::vector<double> inputs = {50, 100, 200, 1000};

    auto outputs = neuroevolution::activate(*nn, inputs);
    BOOST_CHECK_EQUAL(8, nn->mHiddenNodes.size());
    //Hidden nodes starts from 8-15
    BOOST_CHECK_EQUAL(2, nn->mNodes[8].inputs.size());
    BOOST_CHECK(ActivationFunctionType::IDENTITY == nn->mNodes[8].accType);
    BOOST_CHECK_EQUAL(0, nn->mNodes[8].bias);
    BOOST_CHECK_EQUAL(9, nn->mNodes[8].inputs[0].first);
    BOOST_CHECK_EQUAL(0.5, nn->mNodes[8].inputs[0].second);
    BOOST_CHECK_EQUAL(0, nn->mNodes[8].inputs[1].first);
    BOOST_CHECK_EQUAL(0.5, nn->mNodes[8].inputs[1].second);

    BOOST_CHECK_EQUAL(3, nn->mNodes[9].inputs.size());
    BOOST_CHECK_EQUAL(8, nn->mNodes[9].inputs[0].first);
    BOOST_CHECK_EQUAL(0.5, nn->mNodes[9].inputs[0].second);
    BOOST_CHECK_EQUAL(0, nn->mNodes[9].inputs[1].first);
    BOOST_CHECK_EQUAL(0.5, nn->mNodes[9].inputs[1].second);
    BOOST_CHECK_EQUAL(0, nn->mNodes[9].inputs[2].first);
    BOOST_CHECK_EQUAL(0.5, nn->mNodes[9].inputs[2].second);

    BOOST_CHECK_EQUAL(1, nn->mNodes[10].inputs.size());
    BOOST_CHECK_EQUAL(11, nn->mNodes[10].inputs[0].first);
    BOOST_CHECK_EQUAL(0.5, nn->mNodes[10].inputs[0].second);

    BOOST_CHECK_EQUAL(1, nn->mNodes[11].inputs.size());
    BOOST_CHECK_EQUAL(10, nn->mNodes[11].inputs[0].first);
    BOOST_CHECK_EQUAL(0.5, nn->mNodes[11].inputs[0].second);
}

BOOST_FIXTURE_TEST_CASE( Crossover, SnakeGATest ) 
{
    snakega::Genom minimal = snakega::Genom::createMinimal(4, 4);
    populateGenomWithConnections(minimal);
    for(auto c = minimal.beginConnections(); c != minimal.endConnections(); ++c)
    {
        minimal.updateWeight(c, 0);
    }

    for(auto c = minimal.beginConnections(); c != minimal.endConnections(); ++c)
    {
        BOOST_CHECK_EQUAL(0, c->weight);
    }

    snakega::Genom minimal2 = minimal;
    for(auto c = minimal2.beginConnections(); c != minimal2.endConnections(); ++c)
    {
        minimal2.updateWeight(c, 1.0);
    }

    for(auto c = minimal2.beginConnections(); c != minimal2.endConnections(); ++c)
    {
        BOOST_CHECK_EQUAL(1.0, c->weight);
    }

    minimal2.crossoverParametersFrom(minimal);

    for(auto c = minimal2.beginConnections(); c != minimal2.endConnections(); ++c)
    {
        BOOST_CHECK_EQUAL(0.5, c->weight);
    }
}

void printConnections(snakega::Genom& g)
{
    std::cout << "------------------------------------------" << std::endl;
    int i = 0;
    for(auto iter = g.beginConnections(); iter != g.endConnections(); ++iter )
    {
        std::cout << i << "(" << iter->srcNodeId.blockId << ":" << iter->srcNodeId.localId
         << "-" << iter->dstNodeId.blockId << ":" << iter->dstNodeId.localId
         << ") ";

        i++;
    }

    std::cout << std::endl;
}

BOOST_FIXTURE_TEST_CASE( MutateNewDefinition, SnakeGATest ) 
{
    snakega::Genom minimal = snakega::Genom::createMinimal(4, 4);
    populateGenomWithConnections(minimal);

    for(int i = 0; i < 25; ++i)
    {
        auto oldDefsSize = minimal.mDefs.size();
        auto oldBlocksSize = minimal.mBlocks.size();
        auto numCons = minimal.getComplexity();

        if(!minimal.mutateNewDefinition())
        {
            i--;
            continue;
        }

        auto& newBlock = *std::max_element(minimal.mBlocks.begin(), minimal.mBlocks.end(), [](auto x, auto y){return x.blockId < y.blockId;});

        //detect if it was new definition or instantiation
        if(oldDefsSize != minimal.mDefs.size())
        {
            BOOST_CHECK_EQUAL(1, minimal.mDefs.back().neurons.size());
            BOOST_CHECK_EQUAL(1 + oldBlocksSize, minimal.mBlocks.size());
            BOOST_CHECK_EQUAL(newBlock.definitionId, minimal.mDefs.back().id);
            BOOST_CHECK_EQUAL(2, newBlock.externalConnections.size());

            BOOST_CHECK_EQUAL(numCons + 2 - 1, minimal.getComplexity());

            auto expectedNewNode = snakega::Genom::GlobalNodeId{snakega::Genom::GlobalNodeType::Local, newBlock.blockId, 0};
            BOOST_CHECK(expectedNewNode == newBlock.externalConnections[0].dstNodeId);
            BOOST_CHECK(expectedNewNode == newBlock.externalConnections[1].srcNodeId);
        }
        else
        {
            BOOST_CHECK_EQUAL(1 + oldBlocksSize, minimal.mBlocks.size());
            auto defPos = std::find_if(minimal.mDefs.begin(), minimal.mDefs.end(), [=](auto x){return x.id == newBlock.definitionId;});
            BOOST_CHECK(defPos != minimal.mDefs.end());
            BOOST_CHECK_EQUAL(2, newBlock.externalConnections.size());

            BOOST_CHECK_EQUAL(numCons + 2 + defPos->internalConnections.size() - 1, minimal.getComplexity());

            BOOST_CHECK(newBlock.externalConnections[1].srcNodeId == newBlock.externalConnections[0].dstNodeId);
            auto localId = newBlock.externalConnections[1].srcNodeId.localId;
            BOOST_CHECK(defPos->neurons.end() != std::find_if(defPos->neurons.begin(), defPos->neurons.end(), [=](auto x){return x.id == localId;}));
        }
    }

    //Check network is valid
    auto nn = snakega::GenomDecoder::decode(minimal);
    std::vector<double> inputs = {50, 100, 200, 1000};

    auto outputs = neuroevolution::activate(*nn, inputs);
}

BOOST_FIXTURE_TEST_CASE( MutateRemoveLeaf, SnakeGATest ) 
{
    snakega::Genom minimal = snakega::Genom::createMinimal(4, 4);
    populateGenomWithConnections(minimal);

    while(minimal.mBlocks.size() != 1)
    {
        minimal.mutateRemoveLeafBlock();
    }

    BOOST_CHECK_EQUAL(1, minimal.mBlocks.size());
    BOOST_CHECK_EQUAL(1, minimal.mDefs.size());

    //Check network is valid
    auto nn = snakega::GenomDecoder::decode(minimal);
    std::vector<double> inputs = {50, 100, 200, 1000};

    auto outputs = neuroevolution::activate(*nn, inputs);
}

BOOST_FIXTURE_TEST_CASE( MutateNewNeuron, SnakeGATest ) 
{
    snakega::Genom minimal = snakega::Genom::createMinimal(4, 4);
    populateGenomWithConnections(minimal);

    for(int i = 0; i < 25; ++i)
    {
        auto oldNeurons = minimal.getNumNeurons();
        auto oldConns = minimal.getComplexity();   

        minimal.mutateNewNeuron();

        BOOST_CHECK(minimal.getNumNeurons() > oldNeurons);
        BOOST_CHECK(minimal.getComplexity() >= oldConns);
    }

    //Check network is valid
    auto nn = snakega::GenomDecoder::decode(minimal);
    std::vector<double> inputs = {50, 100, 200, 1000};

    auto outputs = neuroevolution::activate(*nn, inputs);
}

BOOST_FIXTURE_TEST_CASE( GetDisconnectedNeurons, SnakeGATest ) 
{
    {
        snakega::Genom minimal = snakega::Genom::createMinimal(4, 4);
        populateGenomWithNodes(minimal);
        BOOST_CHECK_EQUAL(16, minimal.getDisconnectedNeuronIds().size());

        for(int i = 0; i < 8; ++i)
        {
            minimal.erase(minimal.getDisconnectedNeuronIds()[0]);
        }

        BOOST_CHECK_EQUAL(0, minimal.getDisconnectedNeuronIds().size());
    }

    {
        snakega::Genom minimal = snakega::Genom::createMinimal(4, 4);
        populateGenomWithConnections(minimal);
        BOOST_CHECK_EQUAL(0, minimal.getDisconnectedNeuronIds().size());
    }
}

BOOST_FIXTURE_TEST_CASE( MutateNewConnection, SnakeGATest )
{
    {
        snakega::Genom minimal = snakega::Genom::createMinimal(4, 4);
        populateGenomWithNodes(minimal);

        for(int i = 0; i < 25; ++i)
        {
            auto oldConns = minimal.getComplexity();   
            minimal.mutateNewConnection();
            BOOST_CHECK(minimal.getComplexity() > oldConns);
        }
    }
    {
        snakega::Genom minimal = snakega::Genom::createMinimal(4, 4);
        populateGenomWithConnections(minimal);

        for(int i = 0; i < 25; ++i)
        {
            auto oldConns = minimal.getComplexity();   
            minimal.mutateNewConnection();
            BOOST_CHECK(minimal.getComplexity() > oldConns);
        }
    }
}

BOOST_FIXTURE_TEST_CASE( SaveLoadTest, SnakeGATest )
{
    snakega::Config cfg{10, 3, 3, 10, 0.25, 1};
    neuroevolution::DomainGeometry dg {{1, 1, 1}, {{1, 1}, {1, 1}, {1, 1}, {1, 1}}, {{1, 1}, {1, 1}, {1, 1}, {1, 1}}};
    TestFitnessEvaluator eval;

    snakega::Algorithm alg(cfg, dg, eval);

    for(int i = 0; i < 25; ++i)
    {
        alg.step();
    }

    alg.saveState("test.state");
    
    {
        snakega::Algorithm alg2(cfg, dg, eval);
        alg2.loadState("test.state");

        auto& p1 = alg.getPopulation();
        auto& p2 = alg2.getPopulation();

        BOOST_CHECK_EQUAL(p1.size(), p2.size());

        for(std::size_t i = 0; i < p1.size(); ++i)
        {
            BOOST_CHECK_EQUAL(p1[i].fitness, p2[i].fitness);

            BOOST_CHECK(p1[i].mGenom == p2[i].mGenom);
        }
    }
}