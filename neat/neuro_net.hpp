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

   void activate();
   
   InputIterator begin_input();
   InputIterator end_input();

   OutputIterator begin_output();
   OutputIterator end_output();

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

   std::vector<Node*> mHiddenNodes;
   std::vector<Node*> mInputNodes;
   std::vector<Node*> mOutputNodes;
};

}