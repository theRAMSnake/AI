#pragma once
#include <vector>

namespace neat
{

using NodeId = unsigned int;
using InnovationNumber = unsigned int;
using Fitness = int;

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

    Genom(const NodeId numInputs, const NodeId numOutputs);

    static Genom createMinimal(const NodeId numInputs, const NodeId numOutputs);
    static Genom crossover(const Genom& a, const Genom& b, const Fitness fitA, const Fitness fitB);

    void operator += (const Gene& g);
    Gene& operator[] (const std::size_t index);
    std::size_t length();

    NodeId addNode();

    Iterator begin();
    Iterator end();

    NodeId getTotalNodeCount() const;
    NodeId getInputNodeCount() const;
    bool isOutputNode(const NodeId n) const;

private:

    //NodeId convention is [input][output][hidden]
    NodeId mNumInputNodes = 0;
    NodeId mNumOutputNodes = 0;
    NodeId mNumHiddenNodes = 0;
    std::vector<Gene> mGenes;
};

void mutateWeights(Genom& a);
bool mutateAddConnection(Genom& a, InnovationNumber& innovationNumber);
void mutateAddNode(Genom& a, InnovationNumber& innovationNumber);

void mutate(Genom& a, InnovationNumber& innovationNumber);

}