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
    unsigned int population;
    double compatibilityFactor;
    double inheritDisabledChance;
    double perturbationChance;
    double addNodeMutationChance;
    double addConnectionMutationChance;
    double removeConnectionMutationChance;
    double removeNodeMutationChance = 0.05;
    double weightsMutationChance;
    double interspecieCrossoverPercentage = 1;
    double C1_C2;
    double C3;
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

enum class Mutation
{
    Weigths = 1,
    AddNode = 2,
    RemoveNode = 4,
    AddConnection = 8,
    RemoveConnection = 16,
    Complexifying = Weigths | AddNode | AddConnection,
    Simplifying = Weigths | RemoveNode | RemoveConnection,
    All = Weigths | AddNode | RemoveNode | AddConnection | RemoveConnection
};

class Genom
{
public:
    using Iterator = std::vector<Gene>::iterator;
    using ConstIterator = std::vector<Gene>::const_iterator;

    Genom(const NodeId numInputs, const NodeId numOutputs);

    static Genom createMinimal(const NodeId numInputs, const NodeId numOutputs, InnovationHistory& history, const bool connected);
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
    NodeId getNumConnectedHiddenNodes() const;
    NodeId getBiasNodeId() const;
    const std::vector<NodeId>& getInputNodes() const;
    const std::vector<NodeId>& getOutputNodes() const;
    bool isOutputNode(const NodeId n) const;
    bool isInputNode(const NodeId n) const;
    bool isHiddenNode(const NodeId n) const;

    bool isConnected(const NodeId src, const NodeId dst) const;
    void connect(const NodeId src, const NodeId dst, InnovationHistory& history);

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
void mutateRemoveNode(Genom& a, InnovationHistory& history);

void mutate(Genom& a, InnovationHistory& history, const int allowedMutations = static_cast<int>(Mutation::All));

}