#include "neuro_net.hpp"
#include <cmath>
#include <iostream>
#include <algorithm>

namespace neat
{

double activationFunction(const double val)
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
   mOrderedNodes.reserve(totalNodes);
   mNodes.reserve(totalNodes);

   for(NodeId i = 0; i < totalNodes; ++i)
   {
      mNodes.push_back(Node{i, 0.0, 0});
      mOrderedNodes.push_back(i);
   }

   mNodes[mGenotype.getBiasNodeId()].value = 1.0;
   
   for(auto& c : genotype)
   {
      if(c.enabled)
      {
         auto& src = mNodes[c.srcNodeId];
         auto& dst = mNodes[c.dstNodeId];

         dst.inputs.push_back({src.id, c.weight});

         if(src.id != dst.id  && src.depth <= dst.depth)//Otherwise recursive - lets not adapt
         {
            int newDepth = src.depth + 1;
            dst.depth = std::max(newDepth, dst.depth);
         }
      }
   }

   
   std::sort(mOrderedNodes.begin(), mOrderedNodes.end(), [&](auto x, auto y)
   {
      return mNodes[x].depth < mNodes[y].depth;
   });

   /* for(auto n : mOrderedNodes)
   {
      std::cout << n.id << " ";
   }*/
}

std::vector<double> NeuroNet::activateLongTerm(const std::vector<double>& input)
{
   //All nodes should be activate at least once -> this is step
   std::vector<double> output(mGenotype.getOutputNodeCount(), 0.0);

   if(input.size() != mGenotype.getInputNodeCount())
   {
      throw std::invalid_argument("Num inputs does not correspont to genotype");
   }

   //Assign inputs
   int i = 0;
   for(auto& n : mGenotype.getInputNodes())
   {
      auto& node = mNodes[n];
      node.value = input[i++];

      //std::cout << " val: " << node->value;
   }

   //Walk over ordered nodes
   for(auto& id : mOrderedNodes)
   {      
      if(mGenotype.isInputNode(id) || id == mGenotype.getBiasNodeId())
      {
         continue;
      }

      auto& node = mNodes[id];

      //std::cout << " Node: " << n.id;
      double totalInput = 0;
      for(auto& c: node.inputs)
      {
         auto& input = mNodes[c.first];
         
         //std::cout << " Input: " << c.first << " = " << c.second * node->value;
         
         totalInput += c.second * input.value;
      }

      if(!mGenotype.isHiddenNode(id))
      {
         node.value = totalInput;
      }
      else
      {
         node.value = activationFunction(totalInput);
      }
   }

   //Collect outputs
   i = 0;
   for(auto& n : mGenotype.getOutputNodes())
   {
      auto& node = mNodes[n];
      output[i++] = node.value;
   }

   return output;
}  

}