#pragma once
#include <vector>
#include "InnovationHistory.hpp"

namespace neat
{

using Fitness = int;

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

    Genom& operator= (const Genom& other);

    static Genom createMinimal(const NodeId numInputs, const NodeId numOutputs, InnovationHistory& history, const bool connected);
    static Genom crossover(const Genom& a, const Genom& b, const Fitness fitA, const Fitness fitB);
    static double calculateDivergence(const Genom& a, const Genom& b, const double C1_C2, const double C3);
    static Genom read(std::ifstream& s, const NodeId numInputs, const NodeId numOutputs, InnovationHistory& history);

    void write(std::ofstream& s) const;

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

void mutateAddNode(Genom& a, InnovationHistory& history);
void mutateAddConnection(Genom& a, InnovationHistory& history);
    
}

}