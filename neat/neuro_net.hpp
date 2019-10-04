#pragma once
#include "genom.hpp"
#include <vector>
#include <list>
#include <cstddef>

namespace neat
{

class NeuroNet
{
public:
   NeuroNet(const Genom& genotype);

   std::vector<double> activateOneShot(const std::vector<double>& input);
   std::vector<double> activateLongTerm(const std::vector<double>& input);

private:
   struct Node
   {
      NodeId id;
      double value;
      std::vector<std::pair<NodeId, double>> inputs;
   };

   std::list<Node>::iterator getOrCreateNode(const NodeId id);
   void orderNodes(std::list<Node>::iterator first, std::list<Node>::iterator second);

   std::list<Node> mOrderedNodes;
   const Genom& mGenotype;
};

}