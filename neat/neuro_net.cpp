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
   
   mNodes.reserve(totalNodes);
   mInputNodes.reserve(genotype.getInputNodeCount());
   mOutputNodes.reserve(genotype.getOutputNodeCount());
   mHiddenNodes.reserve(genotype.getHiddenNodeCount());

   //here

   for(NodeId i = 0; i < totalNodes; ++i)
   {
      mNodes.push_back(Node{i, 0.0, -1});

      if(mGenotype.isHiddenNode(i) || mGenotype.isOutputNode(i))
      {
         mHiddenAndOutputNodes.push_back(&mNodes.back());
      }
      else if(mGenotype.isInputNode(i))
      {
         mInputNodes.push_back(&mNodes.back());
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
   
   std::sort(mHiddenAndOutputNodes.begin(), mHiddenAndOutputNodes.end(), [&](auto x, auto y)
   {
      return x->depth < y->depth;
   });
}

const std::vector<double>& NeuroNet::getOutput() const
{
   return mOutput;
}

std::vector<double>& NeuroNet::activateLongTerm(const std::vector<double>& input)
{
   if(input.size() != mInputNodes.size())
   {
      throw std::invalid_argument("Num inputs does not correspont to genotype: " + std::to_string(input.size()));
   }

   //Assign inputs
   auto inputIter = input.begin();
   auto inputNodeIter = mInputNodes.begin();

   for(; inputIter != input.end(); ++inputIter, ++inputNodeIter)
   {
      (*inputNodeIter)->value = *inputIter;
   }

   //Walk over ordered hidden and output nodes
   for(auto node : mHiddenAndOutputNodes)
   {      
      double totalInput = 0;
      for(auto& c: node->inputs)
      {
         auto& input = mNodes[c.first];
         totalInput += c.second * input.value;
      }

      if(!mGenotype.isHiddenNode(node->id))
      {
         node->value = totalInput;
      }
      else
      {
         node->value = activationFunction(totalInput);
      }
   }

   //Collect outputs
   std::size_t i = 0;
   for(auto& n : mGenotype.getOutputNodes())
   {
      auto& node = mNodes[n];
      mOutput[i++] = node.value;
   }

   return mOutput;
}  

}