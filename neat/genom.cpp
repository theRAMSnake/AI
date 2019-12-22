#include "genom.hpp"
#include "rng.hpp"
#include <algorithm>
#include <numeric>
#include <set>
#include <string>
#include <iostream>
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
            g.mGenes.back().enabled = a.mGenes[i].enabled && b.mGenes[i].enabled;

            if (!g.mGenes.back().enabled)
            {
                if (!a.mGenes[i].enabled && !b.mGenes[i].enabled)
                {
                    g.mGenes.back().enabled = false;
                }
                else
                {
                    g.mGenes.back().enabled = !Rng::genProbability(mConfig.inheritDisabledChance);
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

Genom Genom::createMinimal(const NodeId numInputs, const NodeId numOutputs, InnovationHistory& history, const bool connected)
{
    Genom g(numInputs, numOutputs);

    if(connected)
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
    for (auto& x : a)
    {
        if (Rng::genProbability(Genom::getConfig().perturbationChance))
        {
            x.weight += Rng::genPerturbation();
        }
        else
        {
            x.weight = Rng::genWeight();
        }
    }
}

bool mutateAddConnection(Genom& a, InnovationHistory& history)
{
    std::vector<std::pair<NodeId, NodeId>> possibleConnections;

    std::vector<NodeId> possibleHiddenNodes;
    for (auto& g : a)
    {
        if (g.enabled)
        {
            if (a.isHiddenNode(g.srcNodeId) || a.isHiddenNode(g.dstNodeId))
            {
                possibleHiddenNodes.push_back(g.srcNodeId);
            }
        }
    }

    //find all possible connections
    for(NodeId src = 0; src < a.getTotalNodeCount(); ++src)
    {
        std::vector<Gene> genes;
        std::copy_if(a.begin(), a.end(), std::back_inserter(genes), [&](auto g){return g.srcNodeId == src && g.enabled;});

        for(NodeId dst = a.getInputNodeCount() + 1; dst < a.getTotalNodeCount(); ++dst)
        {
            if (a.isHiddenNode(dst) && std::find(possibleHiddenNodes.begin(), possibleHiddenNodes.end(), dst) == possibleHiddenNodes.end())
            {
                continue;
            }

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

        a.connect(newConnection.first, newConnection.second, history);
        
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

    if(g.srcNodeId >= getTotalNodeCount())
    {
        mNumHiddenNodes += g.srcNodeId - getTotalNodeCount() + 1;
        mNumTotalNodes += g.srcNodeId - getTotalNodeCount() + 1;
    }

    if(g.dstNodeId >= getTotalNodeCount())
    {
        mNumHiddenNodes += g.dstNodeId - getTotalNodeCount() + 1;
        mNumTotalNodes += g.dstNodeId - getTotalNodeCount() + 1;
    }
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
    std::vector<Gene*> enabled;
    for(auto& x : a)
    {
        if(x.enabled)
        {
            enabled.push_back(&x);
        }
    }

    if(enabled.empty())
    {
        return;
    }

    //std::cout << "-";

    auto randomConnection = enabled[Rng::genChoise(enabled.size())];
    randomConnection->enabled = false;

    auto srcId = randomConnection->srcNodeId;
    auto dstId = randomConnection->dstNodeId;
    auto oldWeight = randomConnection->weight;
    auto newNodeId = a.addNode();

    a += Gene({srcId, newNodeId, true, history.get(srcId, newNodeId), 1.0});
    a += Gene({newNodeId, dstId, true, history.get(newNodeId, dstId), oldWeight});
}

void mutate(Genom& a, InnovationHistory& history, const int allowedMutations)
{
    
    if(allowedMutations & static_cast<int>(Mutation::AddNode) && Rng::genProbability(Genom::getConfig().addNodeMutationChance) && a.length() != 0)
    {
        //std::cout << "+";
        mutateAddNode(a, history);
    }
    if(allowedMutations & static_cast<int>(Mutation::AddConnection) && Rng::genProbability(Genom::getConfig().addConnectionMutationChance))
    {
        mutateAddConnection(a, history);
    }
    if(allowedMutations & static_cast<int>(Mutation::Weigths) && Rng::genProbability(Genom::getConfig().weightsMutationChance) && a.length() != 0)
    {
        mutateWeights(a);
    }
    if(allowedMutations & static_cast<int>(Mutation::RemoveConnection) && Rng::genProbability(Genom::getConfig().removeConnectionMutationChance) && a.length() != 0)
    {
        mutateRemoveConnection(a);
    }
    if(Rng::genProbability(allowedMutations & static_cast<int>(Mutation::RemoveNode) && Genom::getConfig().removeNodeMutationChance) && a.length() != 0)
    {
        mutateRemoveNode(a, history);
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

    if(i != 0)
    {
        weightDiff /= i;
    }

    return  ((double)numDisjointAndExscess * mConfig.C1_C2) / N + mConfig.C3 * weightDiff;
}

NodeId Genom::getOutputNodeCount() const
{
    return mOutputNodes.size();
}

NodeId Genom::getHiddenNodeCount() const
{
    return mNumHiddenNodes;
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

std::size_t Genom::getComplexity() const
{
    return std::accumulate(begin(), end(), 0, [](auto a, auto b){return a + (b.enabled ? 1 : 0);});
}

NodeId Genom::getNumConnectedHiddenNodes() const
{
    std::set<NodeId> nodes;
    
    for (auto& x : mGenes)
    {
        if (x.enabled)
        {
            if (isHiddenNode(x.srcNodeId))
            {
                nodes.insert(x.srcNodeId);
            }
            if (isHiddenNode(x.dstNodeId))
            {
                nodes.insert(x.dstNodeId);
            }
        }
    }

    return nodes.size();
}

void Genom::connect(const NodeId src, const NodeId dst, InnovationHistory& history)
{
    auto pos = std::find_if(begin(), end(), [=](auto x){return x.srcNodeId == src &&
        dst == x.dstNodeId;});

    if(pos == end())
    {
        auto innovationNumber = history.get(src, dst);
        (*this) += Gene({src, dst, true, innovationNumber, Rng::genWeight()});
    }
    else
    {
        pos->enabled = true;
    }
}

void mutateRemoveNode(Genom& a, InnovationHistory& history)
{
    std::vector<NodeId> candidates;

    for(NodeId i = 0; i < a.getTotalNodeCount(); ++i)
    {
        if(!a.isHiddenNode(i))
        {
            continue;
        }

        std::vector<Gene> srcConnections;
        std::vector<Gene> dstConnections;

        std::copy_if(a.begin(), a.end(), std::back_inserter(srcConnections), [&](auto g){return g.enabled && g.dstNodeId == i;});
        std::copy_if(a.begin(), a.end(), std::back_inserter(dstConnections), [&](auto g){return g.enabled && g.srcNodeId == i;});

        //NOTE: update this assumptions once Node genes will be up
        if((srcConnections.size() > 1 && dstConnections.size() > 1) ||
            (srcConnections.empty() && dstConnections.empty())) 
        {
            continue;
        }

        candidates.push_back(i);
    }

    if(!candidates.empty())
    {
        auto nodeId = candidates[Rng::genChoise(candidates.size())];

        std::vector<NodeId> srcIds;
        std::vector<NodeId> dstIds;

        for(auto& g: a)
        {
            if(!g.enabled || (g.srcNodeId != nodeId && g.dstNodeId != nodeId))
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

            g.enabled = false;
        }

        for(auto src : srcIds)
        {
            for(auto dst : dstIds)
            {
                a.connect(src, dst, history);
            }
        }
    }
}

bool Genom::isConnected(const NodeId src, const NodeId dst) const
{
    auto g = std::find_if(begin(), end(), [&](auto x){return x.dstNodeId == dst && x.srcNodeId == src;});
    if(g != end())
    {
        return g->enabled;
    }
    else
    {
        return false;
    }
}

}

