#include "genom.hpp"
#include "rng.hpp"
#include <algorithm>

namespace neat
{

void Genom::setConfig(const Config& c)
{
    mConfig = c;
}

Config& Genom::getConfig()
{
    return mConfig;
}

Genom Genom::crossover(const Genom& a, const Genom& b, const Fitness fitA, const Fitness fitB)
{
    //Pick shared part till innovations match (pick random weight)
    //Pick excess part from most fit (or random in case of same fitness)
    //Gene MIGHT be disabled if it is disabled in one of the parents
    Genom g(a.mInputNodes.size(), a.mOutputNodes.size());

    const bool aIsFittier = fitA == fitB ? Rng::genProbability(0.5) : fitA > fitB;
    const Genom& fittest = aIsFittier ? a : b;

    g.mGenes.reserve(fittest.mGenes.size());

    std::size_t divergencePoint = 0;

    for(std::size_t i = 0; i < std::min(a.mGenes.size(), b.mGenes.size()); ++i)
    {
        if(a.mGenes[i].innovationNumber == b.mGenes[i].innovationNumber)
        {
            g.mGenes.push_back(a.mGenes[i]);
            g.mGenes.back().weight = Rng::genProbability(0.5) > 0 ? a.mGenes[i].weight : b.mGenes[i].weight;

            if(!a.mGenes[i].enabled || !b.mGenes[i].enabled)
            {
                if(Rng::genProbability(mConfig.inheritDisabledChance))
                {
                    g.mGenes.back().enabled = false;
                }
            }
        }
        else
        {
            divergencePoint = i;
            break;
        }
    }

    if(divergencePoint != 0)
    {
        g.mGenes.insert(g.mGenes.end(), fittest.mGenes.begin() + divergencePoint, fittest.mGenes.end());
    }
    else
    {
        if(g.mGenes.size() != fittest.mGenes.size())
        {
            g.mGenes.insert(g.mGenes.end(), fittest.mGenes.begin() + g.mGenes.size(), fittest.mGenes.end());
        }
    }

    g.mNumHiddenNodes = fittest.mNumHiddenNodes;
    g.mNumTotalNodes = fittest.mNumTotalNodes;

    return g;
}

Genom::Genom(const NodeId numInputs, const NodeId numOutputs)
{
    mGenes.reserve(numInputs * numOutputs);

    for(NodeId i = 0; i < numInputs; ++i)
    {
        mInputNodes.push_back(i + 1);
    }
    for(NodeId i = 0; i < numOutputs; ++i)
    {
        mOutputNodes.push_back(i + 1 + numInputs);
    }

    mNumHiddenNodes = 0;
    mNumTotalNodes = 1 + numInputs + numOutputs;
}

Genom Genom::createMinimal(const NodeId numInputs, const NodeId numOutputs, InnovationHistory& history)
{
    Genom g(numInputs, numOutputs);

    if(mConfig.startConnected)
    {
        for(NodeId i = 0; i < numInputs; ++i)
        {
            for(NodeId j = 0; j < numOutputs; ++j)
            {
                g.mGenes.push_back({i + 1, j + numInputs + 1, true, history.get(i + 1, j + numInputs + 1), Rng::genWeight()});
            }
        }
    }
    
    return g;
}

Genom::Iterator Genom::begin()
{
    return mGenes.begin();
}

Genom::Iterator Genom::end()
{
    return mGenes.end();
}

Genom::ConstIterator Genom::begin() const
{
    return mGenes.begin();
}

Genom::ConstIterator Genom::end() const
{
    return mGenes.end();
}

NodeId Genom::getTotalNodeCount() const
{
    return mNumTotalNodes;
}

NodeId Genom::getInputNodeCount() const
{
    return mInputNodes.size();
}

void mutateWeights(Genom& a)
{
    auto& g = a[Rng::genChoise(a.length())];

    if(Rng::genProbability(Genom::getConfig().perturbationChance))
    {
        g.weight += Rng::genPerturbation();
    }
    else
    {
        g.weight = Rng::genWeight();
    }
}

bool mutateAddConnection(Genom& a, InnovationHistory& history)
{
    std::vector<std::pair<NodeId, NodeId>> possibleConnections;

    //find all possible connections
    for(NodeId src = 0; src < a.getTotalNodeCount(); ++src)
    {
        if(a.isOutputNode(src))
        {
            continue;
        }

        std::vector<Gene> genes;
        std::copy_if(a.begin(), a.end(), std::back_inserter(genes), [&](auto g){return g.srcNodeId == src && g.enabled;});

        for(NodeId dst = a.getInputNodeCount() + 1; dst < a.getTotalNodeCount(); ++dst)
        {
            if(std::find_if(genes.begin(), genes.end(), [=](auto x){return x.dstNodeId == dst;}) == genes.end())
            {
                possibleConnections.push_back(std::make_pair(src, dst));
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

        auto pos = std::find_if(a.begin(), a.end(), [=](auto x){return x.srcNodeId == newConnection.first &&
            newConnection.second == x.dstNodeId;});

        if(pos == a.end())
        {
            auto innovationNumber = history.get(newConnection.first, newConnection.second);
            a += Gene({newConnection.first, newConnection.second, true, innovationNumber, Rng::genWeight()});
        }
        else
        {
            pos->enabled = true;
        }
        
        return true;
    }
}

void mutateRemoveConnection(Genom& a)
{
    std::vector<Gene> genes;
    std::copy_if(a.begin(), a.end(), std::back_inserter(genes), [&](auto g){return g.enabled;});

    if(!genes.empty())
    {
        auto& c = genes[Rng::genChoise(genes.size())];
        auto pos = std::find_if(a.begin(), a.end(), [=](auto x){return x.innovationNumber == c.innovationNumber;});
        pos->enabled = false;
    }
}

bool Genom::isOutputNode(const NodeId n) const
{
    return n >= mInputNodes.size() + 1 && n < mInputNodes.size() + 1 + mOutputNodes.size();
}

void Genom::operator += (const Gene& g)
{
    mGenes.push_back(g);
}

Gene& Genom::operator[] (const std::size_t index)
{
    return mGenes[index];
}

const Gene& Genom::operator[] (const std::size_t index) const
{
    return mGenes[index];
}

std::size_t Genom::length() const
{
    return mGenes.size();
}

NodeId Genom::addNode()
{
    mNumHiddenNodes++;
    return mNumTotalNodes++;
}

void mutateAddNode(Genom& a, InnovationHistory& history)
{
    auto& randomConnection = a[Rng::genChoise(a.length())];
    randomConnection.enabled = false;

    auto srcId = randomConnection.srcNodeId;
    auto dstId = randomConnection.dstNodeId;
    auto oldWeight = randomConnection.weight;
    auto newNodeId = a.addNode();

    a += Gene({srcId, newNodeId, true, history.get(srcId, newNodeId), 1.0});
    a += Gene({newNodeId, dstId, true, history.get(newNodeId, dstId), oldWeight});
}

void mutate(Genom& a, InnovationHistory& history)
{
    if(a.length() == 0)
    {
        mutateAddConnection(a, history);
    }
    if(Rng::genProbability(Genom::getConfig().addNodeMutationChance))
    {
        mutateAddNode(a, history);
    }
    if(Rng::genProbability(Genom::getConfig().addConnectionMutationChance))
    {
        mutateAddConnection(a, history);
    }
    if(Rng::genProbability(Genom::getConfig().weightsMutationChance))
    {
        mutateWeights(a);
    }
    if(Rng::genProbability(Genom::getConfig().removeConnectionMutationChance))
    {
        mutateRemoveConnection(a);
    }
}

double Genom::calculateDivergence(const Genom& a, const Genom& b)
{
    std::size_t N = 1;
    
    double weightDiff = 0.0;
    auto iterA = a.begin();
    auto iterB = b.begin();

    std::size_t i = 0;
    for(; iterA != a.end() && iterB != b.end(); ++iterA, ++iterB, i++)
    {
        if(iterA->innovationNumber == iterB->innovationNumber)
        {
            weightDiff += std::abs(a[i].weight - b[i].weight);
        }
        else
        {
            break;
        }
    }

    auto numDisjointAndExscess = std::max(std::distance(iterA, a.end()), std::distance(iterB, b.end()));
    weightDiff /= i;

    return  ((double)numDisjointAndExscess * mConfig.C1_C2) / N + mConfig.C3 * weightDiff;
}

NodeId Genom::getOutputNodeCount() const
{
    return mOutputNodes.size();
}

NodeId Genom::getBiasNodeId() const
{
    return 0;
}

const std::vector<NodeId>& Genom::getInputNodes() const
{
    return mInputNodes;
}

bool Genom::isInputNode(const NodeId n) const
{
    return n > 0 && n < mInputNodes.size() + 1;
}

const std::vector<NodeId>& Genom::getOutputNodes() const
{
    return mOutputNodes;
}

bool Genom::isHiddenNode(const NodeId n) const
{
    return n >= mOutputNodes.size() + 1 + mInputNodes.size();
}

Config Genom::mConfig = {};

}

