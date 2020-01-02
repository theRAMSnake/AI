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

namespace v2
{

enum class ActivationFunctionType
{
    SIGMOID
};

struct ConnectionGene
{
    NodeId srcNodeId;
    NodeId dstNodeId;
    InnovationNumber innovationNumber;
    double weight;
};

struct NodeGene
{
    NodeId id;
    ActivationFunctionType acType;
    unsigned int numConnections;
};

struct MutationConfig
{
    double perturbationChance = 0.0;
    double addNodeMutationChance = 0.0;
    double addConnectionMutationChance = 0.0;
    double removeConnectionMutationChance = 0.0;
    double removeNodeMutationChance = 0.0;
    double weightsMutationChance = 0.0;
};

class Genom
{
public:
    using ConstConnectionsIterator = std::vector<ConnectionGene>::const_iterator;

    enum NodeType
    {
        Bias = 1,
        Input = 2,
        Output = 4,
        Hidden = 8,
        All = Bias | Input | Output | Hidden
    };

    class NodesIterator
    {
    public:
        NodesIterator(const Genom& genom, const NodeType type, const bool isEnd);

        bool operator != (const NodesIterator& other) const;
        bool operator == (const NodesIterator& other) const;
        NodesIterator& operator ++();
        const NodeGene* operator -> () const;

    private:
        void selectNextType();
        std::size_t getElementCount();

        const Genom* mGenom;
        const NodeType mTypes;
        std::size_t mElementIndex = 0;
        NodeType mCurType;
        bool mIsEnd;

        mutable NodeGene mFakeGene;
    };
    friend class NodesIterator;

    Genom(const NodeId numInputs, const NodeId numOutputs);
    Genom(const neat::Genom& older);

    static Genom createMinimal(const NodeId numInputs, const NodeId numOutputs, InnovationHistory& history, const bool connected);
    static Genom crossover(const Genom& a, const Genom& b, const Fitness fitA, const Fitness fitB);
    static double calculateDivergence(const Genom& a, const Genom& b, const double C1_C2, const double C3);

    std::size_t getComplexity() const;

    const ConnectionGene& operator[] (const std::size_t index) const;
    void setWeight(const std::size_t index, const double weight);

    ConstConnectionsIterator begin() const;
    ConstConnectionsIterator end() const;

    NodesIterator beginNodes(const NodeType filter) const;
    NodesIterator endNodes(const NodeType filter) const;
    std::size_t getNodeCount(const NodeType filter) const;

    void mutate(const MutationConfig& config, InnovationHistory& history);

    bool isConnected(const NodeId src, const NodeId dst) const;
    void connect(const NodeId src, const NodeId dst, InnovationHistory& history);
    void disconnect(const NodeId src, const NodeId dst);
    void disconnectAll(); //All unconnected elements will remain!

private:
    void mutateWeights(const double perturbationChance);
    bool mutateAddConnection(InnovationHistory& history);
    void mutateRemoveConnection();
    void mutateAddNode(InnovationHistory& history);
    void mutateRemoveNode(InnovationHistory& history);

    const NodeId mNumBiasNodes;
    const NodeId mNumInputs;
    const NodeId mNumOutputs;

    std::vector<NodeGene> mNodes;
    std::vector<ConnectionGene> mGenes;
};
    
}

}