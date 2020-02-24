#pragma once
#include <list>
#include "node.hpp"

namespace seg
{

class Graph
{
public:
   //Create with random choise
   Graph(const unsigned int numCommands);

   GraphNode& root();
   GraphNode& get(const NodeId id);
   GraphNode& randomNode();
   NodeId addNode(const Payload& payload);
   void removeIfUnreferenced(const NodeId id);
   void fixReferences(const NodeId from, const NodeId to);

   const GraphNode& root() const;
   const GraphNode& get(const NodeId id) const;

   std::size_t size() const;

   void print() const;

private:
   NodeId mNextNodeId = 0;
   std::vector<GraphNode> mNodes;
};

}