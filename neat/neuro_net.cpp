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
   for(auto& c : genotype)
   {
      if(c.enabled)
      {
         auto src = getOrCreateNode(c.srcNodeId);
         auto dst = getOrCreateNode(c.dstNodeId);

         dst->inputs.push_back({src->id, c.weight});

         if(src != dst)
         {
            orderNodes(src, dst);
         }
      }
   }

   /*for(auto n : mOrderedNodes)
   {
      std::cout << n.id << " ";
   }*/
}

void NeuroNet::orderNodes(std::list<Node>::iterator first, std::list<Node>::iterator second)
{
   if(std::distance(mOrderedNodes.begin(), first) > std::distance(mOrderedNodes.begin(), second))
   {
      mOrderedNodes.splice(second, mOrderedNodes, first);
   }
}

std::list<NeuroNet::Node>::iterator NeuroNet::getOrCreateNode(const NodeId id)
{
   auto pos = std::find_if(mOrderedNodes.begin(), mOrderedNodes.end(), [=](auto x){return x.id == id;});
   if(pos == mOrderedNodes.end())
   {
      return mOrderedNodes.insert(mOrderedNodes.end(), {id, 0.0});
   }
   else
   {
      return pos;
   }
}

std::vector<double> NeuroNet::activateOneShot(const std::vector<double>& input)
{
   std::vector<double> output(mGenotype.getOutputNodeCount(), 0.0);

   if(input.size() != mGenotype.getInputNodeCount())
   {
      throw std::invalid_argument("Num inputs does not correspont to genotype");
   }

   unsigned int numOutputsReached = 0;
   const unsigned int MAX_STEPS = 20;
   unsigned int step = 0;

   std::vector<double> valuePerNode(mGenotype.getTotalNodeCount(), 0.0);
   valuePerNode[mGenotype.getBiasNodeId()] = 1.0;
   
   auto inputNodes = mGenotype.getInputNodes();
   for(NodeId i = 0; i < inputNodes.size(); ++i)
   {
      valuePerNode[inputNodes[i]] = static_cast<double>(input[i]);
   }

   while(step < MAX_STEPS && numOutputsReached != mGenotype.getOutputNodeCount())
   {
      std::vector<double> inputPerNode(mGenotype.getTotalNodeCount(), 0.0);

      //Calculate inputs
      for(auto &c : mGenotype)
      {
         if(c.enabled)
         {
            inputPerNode[c.dstNodeId] += valuePerNode[c.srcNodeId] * c.weight;
         }
      }

      numOutputsReached = 0;

      //Activate nodes
      for(NodeId i = 0; i < mGenotype.getTotalNodeCount(); ++i)
      {
         if(mGenotype.isOutputNode(i))
         {
            valuePerNode[i] = inputPerNode[i];
            if(valuePerNode[i] != 0)
            {
               numOutputsReached++;
            }

            continue;
         }

         if(mGenotype.isInputNode(i) || i == mGenotype.getBiasNodeId())
         {
            continue;
         }

         valuePerNode[i] = activationFunction(inputPerNode[i]);
      }

      step++;
   }

   auto outputNodes = mGenotype.getOutputNodes();
   for(NodeId i = 0; i < outputNodes.size(); ++i)
   {
      output[i] = valuePerNode[outputNodes[i]];
   }

   return output;
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
      auto node = getOrCreateNode(n);
      node->value = input[i++];

      std::cout << " val: " << node->value;
   }

   getOrCreateNode(mGenotype.getBiasNodeId())->value = 1.0;

   //Walk over ordered nodes
   for(auto& n : mOrderedNodes)
   {      
      std::cout << " Node: " << n.id;
      double totalInput = 0;
      for(auto& c: n.inputs)
      {
         auto node = getOrCreateNode(c.first);
         
         std::cout << " Input: " << c.first << " = " << node->value;
         
         totalInput += c.second * node->value;
      }

      bool isOutput = mGenotype.isOutputNode(n.id);
      if(isOutput)
      {
         n.value = totalInput;
      }
      else
      {
         n.value = activationFunction(totalInput);
      }
   }

   //Collect outputs
   i = 0;
   for(auto& n : mGenotype.getOutputNodes())
   {
      auto node = getOrCreateNode(n);
      output[i++] = node->value;
   }

   return output;
}  

}