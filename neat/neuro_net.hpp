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

   std::vector<double>& activateLongTerm(const std::vector<double>& input);
   const std::vector<double>& getOutput() const;

private:

   struct Node
   {
      NodeId id;
      double value;
      int depth = -1;
      boost::container::small_vector<std::pair<NodeId, double>, 10> inputs;
   };

   const Genom& mGenotype;
   std::vector<Node> mNodes;
   std::vector<double> mOutput;

   std::vector<Node*> mHiddenAndOutputNodes;
   std::vector<Node*> mInputNodes;
};

}