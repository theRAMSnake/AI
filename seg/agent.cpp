#include "agent.hpp"
#include "graph_traverser.hpp"

namespace seg
{

Agent::Agent(const Graph& g, const std::size_t memorySize)
: mGraph(g)
, mMemory(memorySize)
{
}

void Agent::reset()
{
   
}

unsigned int Agent::run(const double* input)
{
    GraphTraverser t(mGraph, mMemory);
    return t.traverseOnce(input);
}

void Agent::toBinaryStream(std::ofstream& stream) const
{
    throw -1;
}

}