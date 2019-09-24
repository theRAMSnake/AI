#include "genom.hpp"
#include <boost/random.hpp>

namespace neat
{

boost::random::mt19937 rng;
boost::uniform_real<> doubleGen(-1.0, 1.0);
boost::uniform_real<> perturbationGen(-0.05, 0.05);
boost::uniform_real<> realGen(0, 1.0);

const double INHERIT_DISABLED_CHANCE = 0.75;
const double PERTURBATION_CHANCE = 0.9;
const double ADD_NODE_MUTATION_CHANCE = 0.05;
const double ADD_CONNECTION_MUTATION_CHANCE = 0.05;
const double WEIGHTS_MUTATION_CHANCE = 0.8;

Genom Genom::crossover(const Genom& a, const Genom& b, const Fitness fitA, const Fitness fitB)
{
    //Pick shared part till innovations match (pick random weight)
    //Pick excess part from most fit (or random in case of same fitness)
    //Gene MIGHT be disabled if it is disabled in one of the parents
    Genom g(a.mNumInputNodes, a.mNumOutputNodes);

    const bool aIsFittier = fitA == fitB ? doubleGen(rng) > 0 : fitA > fitB;
    const Genom& fittest = aIsFittier ? a : b;

    g.mGenes.reserve(fittest.mGenes.size());

    std::size_t divergencePoint = 0;

    for(std::size_t i = 0; i < std::min(a.mGenes.size(), b.mGenes.size()); ++i)
    {
        if(a.mGenes[i].innovationNumber == b.mGenes[i].innovationNumber)
        {
            g.mGenes.push_back(a.mGenes[i]);
            g.mGenes.back().weight = doubleGen(rng) > 0 ? a.mGenes[i].weight : b.mGenes[i].weight;

            if(!a.mGenes[i].enabled || !b.mGenes[i].enabled)
            {
                if(realGen(rng) < INHERIT_DISABLED_CHANCE)
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
    //TemporaryHere
    rng.seed(static_cast<unsigned int>(std::time(0)));

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
            g.mGenes.push_back({i, j + numInputs, true, innovationNumber++, doubleGen(rng)});
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
        if(realGen(rng) < PERTURBATION_CHANCE)
        {
            x.weight += perturbationGen(rng);
        }
        else
        {
            x.weight = doubleGen(rng);
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

        if(genes.size() == a.getTotalNodeCount() - a.getInputNodeCount() - 1)
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
        auto& newConnection = possibleConnections[rng() % possibleConnections.size()];
        a += Gene({newConnection.first, newConnection.second, true, ++innovationNumber, doubleGen(rng)});
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

std::size_t Genom::length()
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
    auto& randomConnection = a[rng() % a.length()];
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
    if(realGen(rng) < ADD_NODE_MUTATION_CHANCE)
    {
        mutateAddNode(a, innovationNumber);
    }
    else if(realGen(rng) < ADD_CONNECTION_MUTATION_CHANCE)
    {
        mutateAddConnection(a, innovationNumber);
    }
    else if(realGen(rng) < WEIGHTS_MUTATION_CHANCE)
    {
        mutateWeights(a);
    }
}

}

