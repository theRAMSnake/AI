#include <vector>

namespace neat
{

using NodeId = int;
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

struct Genom
{
    std::vector<Gene> genes;
};

Genom crossover(const Genom& a, const Genom& b, const Fitness fitA, const Fitness fitB);
Genom createMinimalGenom(const int numInputs, const int numOutputs);
void mutateWights(Genom& a);
void mutateAddConnection(Genom& a);

}