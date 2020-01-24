#include "genom.hpp"
#include "neuroevolution/rng.hpp"
#include <algorithm>
#include <numeric>
#include <set>
#include <string>
#include <iostream>
namespace neat
{

//---------------------------------------------------------------------------------------------------------------------

namespace v2
{

Genom::Genom(const NodeId numInputs, const NodeId numOutputs)
: mNumBiasNodes(1)
, mNumInputs(numInputs)
, mNumOutputs(numOutputs)
{
    mGenes.reserve(numInputs * numOutputs);
}

Genom Genom::createMinimal(const NodeId numInputs, const NodeId numOutputs, InnovationHistory& history, const bool connected)
{
    Genom g(numInputs, numOutputs);

    if(connected)
    {
        for(NodeId i = 0; i < numInputs; ++i)
        {
            for(NodeId j = 0; j < numOutputs; ++j)
            {
                g.mGenes.push_back({i + 1, j + numInputs + 1, history.get(i + 1, j + numInputs + 1), Rng::genWeight()});
            }
        }
    }
    
    return g;
}

Genom Genom::crossover(const Genom& a, const Genom& b, const Fitness fitA, const Fitness fitB)
{
    //Pick shared part at random
    //Pick excess part from most fit (or random in case of same fitness)
    //Pick disjoint part from most fit
    Genom g(a.mNumInputs, a.mNumOutputs);

    const bool aIsFittier = fitA == fitB ? Rng::genProbability(0.5) : fitA > fitB;
    const Genom& fittest = aIsFittier ? a : b;
    const Genom& notFittest = aIsFittier ? b : a;

    g.mGenes = fittest.mGenes;
    g.mNodes = fittest.mNodes;

    auto iterF = g.mGenes.begin();
    auto iterN = notFittest.begin();

    while(iterF != g.mGenes.end() && iterN != notFittest.end())
    {
        if(iterF->innovationNumber == iterN->innovationNumber)
        {
            iterF->weight = Rng::genProbability(0.5) ? iterF->weight : iterN->weight;
            iterF++;
            iterN++;
        }
        else if(iterF->innovationNumber < iterN->innovationNumber)
        {
            iterF++;
        }
        else
        {
            iterN++;
        }
    }

    return g;
}

double Genom::calculateDivergence(const Genom& a, const Genom& b, const double C1_C2, const double C3)
{
    double weightDiff = 0.0;
    std::size_t numSame = 0;
    std::size_t numDisjointAndExcess = 0;

    auto iter1 = a.begin();
    auto iter2 = b.begin();

    while(iter1 != a.end() && iter2 != b.end())
    {
        if(iter1->innovationNumber == iter2->innovationNumber)
        {
            numSame++;
            iter1++;
            iter2++;
        }
        else if(iter1->innovationNumber < iter2->innovationNumber)
        {
            iter1++;
            numDisjointAndExcess++;
        }
        else
        {
            iter2++;
            numDisjointAndExcess++;
        }
    }

    numDisjointAndExcess += std::distance(iter1, a.end()) + std::distance(iter2, b.end());

    if(numSame != 0)
    {
        weightDiff /= numSame;
    }

    return  (double)numDisjointAndExcess * C1_C2 + C3 * weightDiff;
}

std::size_t Genom::getComplexity() const
{
    return mGenes.size();
}

Genom::ConstConnectionsIterator Genom::begin() const
{
    return mGenes.begin();
}

Genom::ConstConnectionsIterator Genom::end() const
{
    return mGenes.end();
}

Genom::NodesIterator Genom::beginNodes(const Genom::NodeType filter) const
{
    return NodesIterator(*this, filter, false);
}

Genom::NodesIterator Genom::endNodes(const Genom::NodeType filter) const
{
    return NodesIterator(*this, filter, true);
}

std::size_t Genom::getNodeCount(const Genom::NodeType filter) const
{
    std::size_t result = 0;

    if(filter & NodeType::Bias)
    {
        result += mNumBiasNodes;
    }
    if(filter & NodeType::Input)
    {
        result += mNumInputs;
    }
    if(filter & NodeType::Output)
    {
        result += mNumOutputs;
    }
    if(filter & NodeType::Hidden)
    {
        result += mNodes.size();
    }

    return result;
}

void Genom::mutate(const MutationConfig& config, InnovationHistory& history)
{
    if(Rng::genProbability(config.addNodeMutationChance) && mGenes.size() != 0)
    {
        mutateAddNode(history);
    }
    if(Rng::genProbability(config.addConnectionMutationChance))
    {
        mutateAddConnection(history);
    }
    if(Rng::genProbability(config.weightsMutationChance) && mGenes.size() != 0)
    {
        mutateWeights(config.perturbationChance);
    }
    if(Rng::genProbability(config.removeConnectionMutationChance) && mGenes.size() != 0)
    {
        mutateRemoveConnection();
    }
    if(Rng::genProbability(config.removeNodeMutationChance) && !mNodes.empty())
    {
        mutateRemoveNode(history);
    }
    if(config.changeNodeMutationChance != 0.0 && Rng::genProbability(config.changeNodeMutationChance) && !mNodes.empty())
    {
        mutateChangeNode();
    }
}

void Genom::mutateChangeNode()
{
    auto& node = mNodes[Rng::genChoise(mNodes.size())];

    node.acType = static_cast<ActivationFunctionType>(Rng::genChoise(NUM_ACTIVATION_FUNCTION_TYPES));
}

void Genom::mutateWeights(const double perturbationChance)
{
    for (auto& x : mGenes)
    {
        if (Rng::genProbability(perturbationChance))
        {
            x.weight += Rng::genPerturbation();
        }
        else
        {
            x.weight = Rng::genWeight();
        }
    }
}

bool Genom::mutateAddConnection(InnovationHistory& history)
{
    std::vector<std::pair<NodeId, NodeId>> possibleConnections;

    //Genom has only good hidden nodes: std::vector<NodeId> possibleHiddenNodes;

    //find all possible connections
    for(auto srcIter = beginNodes(NodeType::All); srcIter != endNodes(NodeType::All); ++srcIter)
    {
        for(auto dstIter = beginNodes(static_cast<NodeType>(NodeType::Output | NodeType::Hidden)); 
            dstIter != endNodes(static_cast<NodeType>(NodeType::Output | NodeType::Hidden)); 
            ++dstIter)
        {
            if(!isConnected(srcIter->id, dstIter->id))
            {
                possibleConnections.push_back({srcIter->id, dstIter->id});
            }
        }
    }
    
    if(possibleConnections.empty())
    {
        return false;
    }
    else
    {
        //choose one at random
        auto& newConnection = possibleConnections[Rng::genChoise(possibleConnections.size())];

        connect(newConnection.first, newConnection.second, history, Rng::genWeight());
        
        return true;
    }
}

void Genom::mutateRemoveConnection()
{
    if(!mGenes.empty())
    {
        auto cNum = Rng::genChoise(mGenes.size());
        auto cIter = mGenes.begin() + cNum;

        disconnect(cIter->srcNodeId, cIter->dstNodeId);
    }
}

void Genom::connect(const NodeId src, const NodeId dst, InnovationHistory& history, double weight)
{
    auto srcpos = std::find_if(mNodes.begin(), mNodes.end(), [=](auto n){return n.id == src;});
    if(srcpos != mNodes.end())
    {
        srcpos->numConnections++;
    }

    auto dstpos = std::find_if(mNodes.begin(), mNodes.end(), [=](auto n){return n.id == dst;});
    if(dstpos != mNodes.end())
    {
        dstpos->numConnections++;
    }

    auto g = ConnectionGene{src, dst, history.get(src, dst), weight};
    mGenes.insert(std::lower_bound(mGenes.begin(), mGenes.end(), g, [](auto x, auto y){return x.innovationNumber < y.innovationNumber;}),
        g);
}

void Genom::disconnect(const NodeId src, const NodeId dst)
{
    auto srcpos = std::find_if(mNodes.begin(), mNodes.end(), [=](auto n){return n.id == src;});
    if(srcpos != mNodes.end())
    {
        srcpos->numConnections--;

        if(srcpos->numConnections == 0)
        {
            mNodes.erase(srcpos);
        }
    }

    auto dstpos = std::find_if(mNodes.begin(), mNodes.end(), [=](auto n){return n.id == dst;});
    if(dstpos != mNodes.end())
    {
        dstpos->numConnections--;

        if(dstpos->numConnections == 0)
        {
            mNodes.erase(dstpos);
        }
    }

    mGenes.erase(std::find_if(begin(), end(), [=](auto n){return n.dstNodeId == dst && n.srcNodeId == src;}));
}

void Genom::mutateRemoveNode(InnovationHistory& history)
{
    std::vector<NodeId> candidates;

    for(auto nodeIter = beginNodes(NodeType::Hidden); nodeIter != endNodes(NodeType::Hidden); ++nodeIter)
    {
        std::vector<ConnectionGene> srcConnections;
        std::vector<ConnectionGene> dstConnections;

        std::copy_if(begin(), end(), std::back_inserter(srcConnections), [&](auto g){return g.dstNodeId == nodeIter->id;});
        std::copy_if(begin(), end(), std::back_inserter(dstConnections), [&](auto g){return g.srcNodeId == nodeIter->id;});

        if((srcConnections.size() > 1 && dstConnections.size() > 1))
        {
            continue;
        }

        candidates.push_back(nodeIter->id);
    }

    if(!candidates.empty())
    {
        auto nodeId = candidates[Rng::genChoise(candidates.size())];

        std::vector<NodeId> srcIds;
        std::vector<NodeId> dstIds;

        for(auto& g: mGenes)
        {
            if(g.srcNodeId != nodeId && g.dstNodeId != nodeId)
            {
                continue;
            }
            else if(g.srcNodeId == nodeId)
            {
                dstIds.push_back(g.dstNodeId);
            }
            else if(g.dstNodeId == nodeId)
            {
                srcIds.push_back(g.srcNodeId);
            }
        }

        for(auto src : srcIds)
        {
            for(auto dst : dstIds)
            {
                connect(src, dst, history, Rng::genWeight());
            }
        }

        for(auto src : srcIds)
        {
            disconnect(src, nodeId);
        }
        for(auto dst : dstIds)
        {
            disconnect(nodeId, dst);
        }

        auto nodePos = std::find_if(mNodes.begin(), mNodes.end(), [=](auto n){return n.id == nodeId;});
        //If node is still there -> erase it
        if(nodePos != mNodes.end())
        {
            mNodes.erase(nodePos);
        }
    }
}

void Genom::mutateAddNode(InnovationHistory& history)
{
    auto pos = Rng::genChoise(mGenes.size());
    auto randomConnection = mGenes[pos];

    auto srcId = randomConnection.srcNodeId;
    auto dstId = randomConnection.dstNodeId;
    auto oldWeight = randomConnection.weight;
    auto newNodeId = randomConnection.innovationNumber + mNumInputs + mNumOutputs + mNumBiasNodes;

    mGenes.erase(mGenes.begin() + pos);

    mNodes.push_back({newNodeId, ActivationFunctionType::SIGMOID, 2});

    connect(srcId, newNodeId, history, 1.0);
    connect(newNodeId, dstId, history, oldWeight);
}

void Genom::NodesIterator::selectNextType()
{
    if(mCurType < NodeType::Bias && (mTypes & NodeType::Bias) != 0)
    {
        mFakeGene.id = 0;
        mCurType = NodeType::Bias;
        mElementIndex = 0;
    }
    else if(mCurType < NodeType::Input && (mTypes & NodeType::Input) != 0)
    {
        mFakeGene.id = mGenom->mNumBiasNodes;
        mCurType = NodeType::Input;
        mElementIndex = 0;
    }
    else if(mCurType < NodeType::Output && (mTypes & NodeType::Output) != 0)
    {
        mFakeGene.id = mGenom->mNumBiasNodes + mGenom->mNumInputs;
        mCurType = NodeType::Output;
        mElementIndex = 0;
    }
    else if(mCurType < NodeType::Hidden && (mTypes & NodeType::Hidden) != 0)
    {
        mCurType = NodeType::Hidden;
        if(mGenom->mNodes.size() == 0)
        {
            mIsEnd = true;
        }
        else
        {
            mElementIndex = 0;
        }
    }
    else
    {
        mIsEnd = true;
    }
}

std::size_t Genom::NodesIterator::getElementCount()
{
    switch(mCurType)
    {
        case NodeType::Bias:
            return mGenom->mNumBiasNodes;

        case NodeType::Input:
            return mGenom->mNumInputs;

        case NodeType::Output:
            return mGenom->mNumOutputs;

        case NodeType::Hidden:
            return mGenom->mNodes.size();

        default: return 0;
    }
}

Genom::NodesIterator::NodesIterator(const Genom& genom, const NodeType type, const bool isEnd)
: mGenom(&genom)
, mTypes(type)
, mCurType(static_cast<NodeType>(0))
, mIsEnd(isEnd)
{
    mFakeGene.acType = ActivationFunctionType::SIGMOID;
    mFakeGene.numConnections = 0;
    mFakeGene.id = 0;

    if(!mIsEnd)
    {
        selectNextType();
    }
}

bool Genom::NodesIterator::operator == (const Genom::NodesIterator& other) const
{
    return (mGenom == other.mGenom &&
        mElementIndex == other.mElementIndex &&
        mCurType == other.mCurType && mIsEnd == other.mIsEnd) || (mIsEnd && other.mIsEnd);
}

bool Genom::NodesIterator::operator != (const Genom::NodesIterator& other) const
{
    return ! operator ==(other);
}

Genom::NodesIterator& Genom::NodesIterator::operator ++()
{
    if(!mIsEnd)
    {
        mFakeGene.id++;
        if(++mElementIndex == getElementCount())
        {
            selectNextType();
        }
    }

    return *this;
}

const NodeGene* Genom::NodesIterator::operator -> () const
{
    if(mCurType == NodeType::Hidden)
    {
        return &mGenom->mNodes[mElementIndex];
    }
    else
    {
        return &mFakeGene;
    }
}

bool Genom::isConnected(const NodeId src, const NodeId dst) const
{
    auto g = std::find_if(begin(), end(), [&](auto x){return x.dstNodeId == dst && x.srcNodeId == src;});
    return g != end();
}

const ConnectionGene& Genom::operator[] (const std::size_t index) const
{
    return mGenes[index];
}

void Genom::setWeight(const std::size_t index, const double weight)
{
    mGenes[index].weight = weight;
}

void Genom::disconnectAll()
{
    mGenes.clear();
}

void mutateAddNode(Genom& a, InnovationHistory& history)
{
    MutationConfig cfg;
    cfg.addNodeMutationChance = 1.0;

    a.mutate(cfg, history);
}

void mutateAddConnection(Genom& a, InnovationHistory& history)
{
    MutationConfig cfg;
    cfg.addConnectionMutationChance = 1.0;

    a.mutate(cfg, history);
}

Genom Genom::read(std::ifstream& s, const NodeId numInputs, const NodeId numOutputs, InnovationHistory& history)
{
    Genom g(numInputs, numOutputs);

    std::size_t genomSize = 0;
    s.read((char*)&genomSize, sizeof(std::size_t));

    for(std::size_t k = 0; k < genomSize; ++k)
    {
        v2::ConnectionGene z;

        s.read((char*)&z.innovationNumber, sizeof(InnovationNumber));
        s.read((char*)&z.weight, sizeof(double));

        auto innovation = history.get(z.innovationNumber);

        z.srcNodeId = innovation.first;
        z.dstNodeId = innovation.second;

        g.mGenes.push_back(z);
    }

    std::size_t nodesSize = 0;
    s.read((char*)&nodesSize, sizeof(std::size_t));

    for(std::size_t k = 0; k < nodesSize; ++k)
    {
        NodeGene z;

        s.read((char*)&z.id, sizeof(NodeId));
        s.read((char*)&z.acType, sizeof(unsigned int));
        s.read((char*)&z.numConnections, sizeof(unsigned int));

        g.mNodes.push_back(z);
    }

    return g;
}

void Genom::write(std::ofstream& s) const
{
    auto genSize = getComplexity();
    s.write((char*)&genSize, sizeof(std::size_t));

    for(auto& z : *this)
    {
        s.write((char*)&z.innovationNumber, sizeof(InnovationNumber));
        s.write((char*)&z.weight, sizeof(double));
    }

    auto nodesSize = getNodeCount(v2::Genom::NodeType::Hidden);
    s.write((char*)&nodesSize, sizeof(std::size_t));

    for(auto iter = beginNodes(v2::Genom::NodeType::Hidden); 
        iter != endNodes(v2::Genom::NodeType::Hidden);
        ++iter)
    {
        s.write((char*)&iter->id, sizeof(NodeId));
        s.write((char*)&iter->acType, sizeof(unsigned int));
        s.write((char*)&iter->numConnections, sizeof(unsigned int));
    }
}

Genom& Genom::operator= (const Genom& other)
{
    mNodes = other.mNodes;
    mGenes = other.mGenes;

    return *this;
}

const NodeGene& Genom::NodesIterator::operator *() const
{
    return *operator->();
}

std::unique_ptr<neuroevolution::NeuroNet> createAnn(const Genom& g)
{
    std::vector<NodeId> inputNodes(g.getNodeCount(neat::v2::Genom::NodeType::Input)); //Always same can be optimized
    std::vector<NodeId> outputNodes(g.getNodeCount(neat::v2::Genom::NodeType::Output)); //Always same can be optimized
    std::vector<neuroevolution::NeuroNet::HiddenNodeDef> hiddenNodes(g.getNodeCount(neat::v2::Genom::NodeType::Hidden) +
        g.getNodeCount(neat::v2::Genom::NodeType::Bias));
    std::vector<neuroevolution::NeuroNet::ConnectionDef> connections(g.getComplexity());

    std::transform(g.beginNodes(neat::v2::Genom::NodeType::Input),
        g.endNodes(neat::v2::Genom::NodeType::Input),
        inputNodes.begin(),
        [](auto x){return x.id;}
        );

    std::transform(g.beginNodes(neat::v2::Genom::NodeType::Bias),
        g.endNodes(neat::v2::Genom::NodeType::Bias),
        hiddenNodes.begin(),
        [](auto x){return neuroevolution::NeuroNet::HiddenNodeDef{x.id, ActivationFunctionType::IDENTITY, 1.0};}
        );

    std::transform(g.beginNodes(neat::v2::Genom::NodeType::Output),
        g.endNodes(neat::v2::Genom::NodeType::Output),
        outputNodes.begin(),
        [](auto x){return x.id;}
        );

    std::transform(g.beginNodes(neat::v2::Genom::NodeType::Hidden),
        g.endNodes(neat::v2::Genom::NodeType::Hidden),
        hiddenNodes.begin(),
        [](auto x){return neuroevolution::NeuroNet::HiddenNodeDef{x.id, x.acType, 0.0};}
        );

    std::transform(g.begin(),
        g.end(),
        connections.begin(),
        [](auto x){return neuroevolution::NeuroNet::ConnectionDef{x.srcNodeId, x.dstNodeId, x.weight};}
        );

    return std::make_unique<neuroevolution::NeuroNet>(inputNodes, outputNodes, hiddenNodes, connections);
}

}

}

