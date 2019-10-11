#pragma once
#include "genom.hpp"
#include <vector>
#include <list>
#include <cstddef>
#include <unordered_map>
#include <boost/container/small_vector.hpp>

namespace neat
{

class NeuroNet
{
public:
   NeuroNet(const Genom& genotype);

   std::vector<double> activateLongTerm(const std::vector<double>& input);

private:

   struct Node
   {
      NodeId id;
      double value;
      int depth = -1;
      boost::container::small_vector<std::pair<NodeId, double>, 10> inputs;
      //std::vector<std::pair<NodeId, double>> inputs;
   };

   std::vector<Node> mNodes;
   std::vector<NodeId> mOrderedNodes;
   const Genom& mGenotype;
};

}