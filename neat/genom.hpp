#pragma once
#include <vector>
#include "InnovationHistory.hpp"

namespace neat
{

using Fitness = int;

struct Config
{
    NodeId numInputs;
    NodeId numOutputs;
    unsigned int initialPopulation;
    unsigned int optimalPopulation;
    double compatibilityFactor;
    double inheritDisabledChance;
    double perturbationChance;
    double addNodeMutationChance;
    double addConnectionMutationChance;
    double removeConnectionMutationChance;
    double weightsMutationChance;
    double adoptionRate;
    double C1_C2;
    double C3;
    bool startConnected = true;
    int numThreads = 1;
};

struct Gene
{
    NodeId srcNodeId;
    NodeId dstNodeId;
    bool enabled;
    InnovationNumber innovationNumber;
    double weight;
};

class Genom
{
public:
    using Iterator = std::vector<Gene>::iterator;
    using ConstIterator = std::vector<Gene>::const_iterator;

    Genom(const NodeId numInputs, const NodeId numOutputs);

    static Genom createMinimal(const NodeId numInputs, const NodeId numOutputs, InnovationHistory& history);
    static Genom crossover(const Genom& a, const Genom& b, const Fitness fitA, const Fitness fitB);
    static double calculateDivergence(const Genom& a, const Genom& b);

    static void setConfig(const Config& c);
    static Config& getConfig();

    void operator += (const Gene& g);
    Gene& operator[] (const std::size_t index);
    const Gene& operator[] (const std::size_t index) const;
    std::size_t length() const;
    std::size_t getComplexity() const;

    NodeId addNode();

    Iterator begin();
    Iterator end();

    ConstIterator begin() const;
    ConstIterator end() const;

    NodeId getTotalNodeCount() const;
    NodeId getInputNodeCount() const;
    NodeId getOutputNodeCount() const;
    NodeId getHiddenNodeCount() const;
    NodeId getBiasNodeId() const;
    const std::vector<NodeId>& getInputNodes() const;
    const std::vector<NodeId>& getOutputNodes() const;
    bool isOutputNode(const NodeId n) const;
    bool isInputNode(const NodeId n) const;
    bool isHiddenNode(const NodeId n) const;

private:

    //NodeId convention is [bias][input][output][hidden]
    //NodeId mNumInputNodes = 0;
    //NodeId mNumOutputNodes = 0;
    std::vector<NodeId> mInputNodes;
    std::vector<NodeId> mOutputNodes;
    NodeId mNumHiddenNodes = 0;
    NodeId mNumTotalNodes = 0;
    std::vector<Gene> mGenes;
    static Config mConfig;
};

void mutateWeights(Genom& a);
bool mutateAddConnection(Genom& a, InnovationHistory& history);
void mutateRemoveConnection(Genom& a);
void mutateAddNode(Genom& a, InnovationHistory& history);

void mutate(Genom& a, InnovationHistory& history);

}