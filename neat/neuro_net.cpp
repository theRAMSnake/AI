#include "neuro_net.hpp"
#include <cmath>
#include <iostream>
#include <algorithm>

namespace neat
{

double activationFunction (const double val)
{
   if(val > 1.5)
   {
      return 1.0;
   }
   else if(val < -1.5)
   {
      return 0;
   }
   else
   {
      return 1.0 / (1 + std::exp(-5.0 * val));
   }
}

NeuroNet::NeuroNet(const Genom& genotype)
: mGenotype(genotype)
{
   auto totalNodes = mGenotype.getTotalNodeCount();
   
   mNodes.reserve(totalNodes);
   mInputNodes.reserve(genotype.getInputNodeCount());
   mOutputNodes.reserve(genotype.getOutputNodeCount());
   mHiddenNodes.reserve(genotype.getHiddenNodeCount());

   for(NodeId i = 0; i < totalNodes; ++i)
   {
      mNodes.push_back(Node{i, 0.0, -1});

      if(mGenotype.isHiddenNode(i))
      {
         mHiddenNodes.push_back(&mNodes.back());
      }
      else if(mGenotype.isInputNode(i))
      {
         mInputNodes.push_back(&mNodes.back());
      }
      else if(mGenotype.isOutputNode(i))
      {
         mOutputNodes.push_back(&mNodes.back());
      }
   }

   for(NodeId i : mGenotype.getInputNodes())
   {
      mNodes[i].depth = 0;
   }

   mNodes[mGenotype.getBiasNodeId()].value = 1.0;
   
   for(auto& c : genotype)
   {
      if(c.enabled)
      {
         auto& src = mNodes[c.srcNodeId];
         auto& dst = mNodes[c.dstNodeId];

         dst.inputs.push_back({src.id, c.weight});

         if(src.id != dst.id && (src.depth <= dst.depth || dst.depth == -1))//Otherwise recursive - lets not adapt
         {
            int newDepth = src.depth + 1;
            dst.depth = std::max(newDepth, dst.depth);
         }
      }
   }
   
   std::sort(mHiddenNodes.begin(), mHiddenNodes.end(), [&](auto x, auto y)
   {
      return x->depth < y->depth;
   });

   std::sort(mOutputNodes.begin(), mOutputNodes.end(), [&](auto x, auto y)
   {
      return x->depth < y->depth;
   });
}

void NeuroNet::activate()
{
   /*
   struct Node
   {
      NodeId id;
      double value;
      int depth = -1;
      boost::container::small_vector<std::pair<NodeId, double>, 10> inputs;
   };
   */

   //Walk over ordered hidden nodes
   for(auto node : mHiddenNodes)
   {      
      double totalInput = 0;
      for(auto& c: node->inputs)
      {
         auto& input = mNodes[c.first];
         totalInput += c.second * input.value;
      }

      node->value = activationFunction(totalInput);
   }

   //Walk over ordered output nodes
   for(auto node : mOutputNodes)
   {      
      double totalInput = 0;
      for(auto& c: node->inputs)
      {
         auto& input = mNodes[c.first];
         totalInput += c.second * input.value;
      }

      node->value = totalInput;
   }
}  

NeuroNet::NodeIterator NeuroNet::begin_input()
{
   return NodeIterator(&mInputNodes.front());
}

NeuroNet::NodeIterator NeuroNet::end_input()
{
   return NodeIterator(&*mInputNodes.end());
}

const NeuroNet::NodeIterator NeuroNet::begin_output() const
{
   return NodeIterator(const_cast<Node**>(&mOutputNodes.front()));
}

const NeuroNet::NodeIterator NeuroNet::end_output() const
{
   return NodeIterator(const_cast<Node**>(&*mOutputNodes.end()));
}

std::vector<double> activate(NeuroNet& n, const std::vector<double>& input)
{
   std::vector<double> result;

   std::copy(input.begin(), input.end(), n.begin_input());

   n.activate();

   std::copy(n.begin_output(), n.end_output(), std::back_inserter(result));

   return result;
}

NeuroNet::NodeIterator::NodeIterator(Node** nodePtr)
: mNodePtr(nodePtr)
{

}

bool NeuroNet::NodeIterator::operator== (const NeuroNet::NodeIterator& other) const
{
   return mNodePtr == other.mNodePtr;
}

bool NeuroNet::NodeIterator::operator!= (const NeuroNet::NodeIterator& other) const
{
   return mNodePtr != other.mNodePtr;
}

NeuroNet::NodeIterator::difference_type NeuroNet::NodeIterator::operator- (const NeuroNet::NodeIterator& other) const
{
   return mNodePtr - other.mNodePtr;
}

}