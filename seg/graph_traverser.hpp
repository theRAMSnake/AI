#pragma once
#include "graph.hpp"
#include "interpreter.hpp"

namespace seg
{

class GraphTraverser
{
public:
    GraphTraverser(
        const Graph& graph,
        Memory& memory
        );

    unsigned int traverseOnce(const double* input);

private:
    const NodeId procChoise(const Choise& ch, Interpreter& inter);

    const Graph& mGraph;
    Memory& mMemory;
};

}