#pragma once
#include "node.hpp"
#include "interpreter.hpp"

namespace seg
{

class GraphTraverser
{
public:
    GraphTraverser(
        const GraphNode& root,
        Memory& memory
        );

    unsigned int traverseOnce(const double* input);

private:
    const GraphNode& mRoot;
    Memory& mMemory;
};

}