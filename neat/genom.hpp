#pragma once
#include <vector>
#include "InnovationHistory.hpp"

namespace neat
{

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
    using ConstIterator = std::vector<Gene>::const_iterator;

    Genom(const NodeId numInputs, const NodeId numOutputs);

    static Genom createMinimal(const NodeId numInputs, const NodeId numOutputs, InnovationHistory& history);
    static Genom crossover(const Genom& a, const Genom& b, const Fitness fitA, const Fitness fitB);
    static double calculateDivergence(const Genom& a, const Genom& b);

    void operator += (const Gene& g);
    Gene& operator[] (const std::size_t index);
    const Gene& operator[] (const std::size_t index) const;
    std::size_t length() const;

    NodeId addNode();

    Iterator begin();
    Iterator end();

    ConstIterator begin() const;
    ConstIterator end() const;

    NodeId getTotalNodeCount() const;
    NodeId getInputNodeCount() const;
    bool isOutputNode(const NodeId n) const;

private:

    //NodeId convention is [bias][input][output][hidden]
    NodeId mNumInputNodes = 0;
    NodeId mNumOutputNodes = 0;
    NodeId mNumHiddenNodes = 0;
    std::vector<Gene> mGenes;
};

void mutateWeights(Genom& a);
bool mutateAddConnection(Genom& a, InnovationHistory& history);
void mutateAddNode(Genom& a, InnovationHistory& history);

void mutate(Genom& a, InnovationHistory& history);

}