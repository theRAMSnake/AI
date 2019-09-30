#include "genom.hpp"
#include "rng.hpp"
#include <algorithm>

namespace neat
{

const double INHERIT_DISABLED_CHANCE = 0.75;
const double PERTURBATION_CHANCE = 0.9;
const double ADD_NODE_MUTATION_CHANCE = 0.05;
const double ADD_CONNECTION_MUTATION_CHANCE = 0.05;
const double WEIGHTS_MUTATION_CHANCE = 0.8;
const double C1_C2 = 1.0;
const double C3 = 2.0;

Genom Genom::crossover(const Genom& a, const Genom& b, const Fitness fitA, const Fitness fitB)
{
    //Pick shared part till innovations match (pick random weight)
    //Pick excess part from most fit (or random in case of same fitness)
    //Gene MIGHT be disabled if it is disabled in one of the parents
    Genom g(a.mNumInputNodes, a.mNumOutputNodes);

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
                if(Rng::genProbability(INHERIT_DISABLED_CHANCE))
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
        g.mNumHiddenNodes = fittest.mNumHiddenNodes;
    }

    return g;
}

Genom::Genom(const NodeId numInputs, const NodeId numOutputs)
{
    mGenes.reserve(numInputs * numOutputs);
    mNumInputNodes = numInputs;
    mNumOutputNodes = numOutputs;
    mNumHiddenNodes = 0;
}

Genom Genom::createMinimal(const NodeId numInputs, const NodeId numOutputs)
{
    Genom g(numInputs, numOutputs);
    g.mGenes.reserve(numInputs * numOutputs);

    InnovationNumber innovationNumber = 0;
    for(NodeId i = 0; i < numInputs; ++i)
    {
        for(NodeId j = 0; j < numOutputs; ++j)
        {
            g.mGenes.push_back({i, j + numInputs, true, innovationNumber++, Rng::genWeight()});
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

NodeId Genom::getTotalNodeCount() const
{
    return mNumHiddenNodes + mNumInputNodes + mNumOutputNodes;
}

NodeId Genom::getInputNodeCount() const
{
    return mNumInputNodes;
}

void mutateWeights(Genom& a)
{
    for(auto& x : a)
    {
        if(Rng::genProbability(PERTURBATION_CHANCE))
        {
            x.weight += Rng::genPerturbation();
        }
        else
        {
            x.weight = Rng::genWeight();
        }
    }
}

bool mutateAddConnection(Genom& a, InnovationNumber& innovationNumber)
{
    std::vector<std::pair<NodeId, NodeId>> possibleConnections;

    //find all possible connections
    for(NodeId src = 0; src < a.getTotalNodeCount(); ++src)
    {
        std::vector<Gene> genes;
        std::copy_if(a.begin(), a.end(), std::back_inserter(genes), [&](auto g){return g.srcNodeId == src && g.enabled;});

        if(genes.size() == a.getTotalNodeCount() - a.getInputNodeCount())
        {
            ///All possible connections are there
            continue;
        }

        for(NodeId dst = a.getInputNodeCount(); dst < a.getTotalNodeCount(); ++dst)
        {
            if(dst != src && std::find_if(genes.begin(), genes.end(), [=](auto x){return x.dstNodeId == dst;}) == genes.end()
                && !(a.isOutputNode(src) && a.isOutputNode(dst)))
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
            a += Gene({newConnection.first, newConnection.second, true, ++innovationNumber, Rng::genWeight()});
        }
        else
        {
            pos->enabled = true;
        }
        
        return true;
    }
}

bool Genom::isOutputNode(const NodeId n) const
{
    return n >= mNumInputNodes && n < mNumInputNodes + mNumOutputNodes;
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
    return getTotalNodeCount() - 1;
}

void mutateAddNode(Genom& a, InnovationNumber& innovationNumber)
{
    auto& randomConnection = a[Rng::genChoise(a.length())];
    randomConnection.enabled = false;

    auto srcId = randomConnection.srcNodeId;
    auto dstId = randomConnection.dstNodeId;
    auto oldWeight = randomConnection.weight;
    auto newNodeId = a.addNode();

    a += Gene({srcId, newNodeId, true, ++innovationNumber, 1.0});
    a += Gene({newNodeId, dstId, true, ++innovationNumber, oldWeight});
}

void mutate(Genom& a, InnovationNumber& innovationNumber)
{
    if(Rng::genProbability(ADD_NODE_MUTATION_CHANCE))
    {
        mutateAddNode(a, innovationNumber);
    }
    else if(Rng::genProbability(ADD_CONNECTION_MUTATION_CHANCE))
    {
        mutateAddConnection(a, innovationNumber);
    }
    else if(Rng::genProbability(WEIGHTS_MUTATION_CHANCE))
    {
        mutateWeights(a);
    }
}

double Genom::calculateDivergence(const Genom& a, const Genom& b)
{
    std::size_t lengthDiff = 0;
    std::size_t N = 0;
    if(a.length() >= b.length())
    {
        lengthDiff = a.length() - b.length();
        N = a.length() > 20 ? a.length() : 1;
    }
    else
    {
        lengthDiff = b.length() - a.length();
        N = b.length() > 20 ? b.length() : 1;
    }

    double weightDiff = 0.0;
    std::size_t i = 0;
    for(; i < a.length() && i < b.length(); ++i)
    {
        weightDiff += std::abs(a[i].weight - b[i].weight);
    }

    weightDiff /= i;

    return  ((double)lengthDiff * C1_C2) / N + C3 * weightDiff;
}

}

