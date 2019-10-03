#include "neuro_net.hpp"
#include <cmath>
#include <iostream>

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

}

std::vector<double> NeuroNet::activateOneShot(const std::vector<double>& input)
{
   std::vector<double> output(mGenotype.getOutputNodeCount());

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

   /*for(NodeId i = 0; i < valuePerNode.size(); ++i)
   {
      std::cout << "Value per node: n = " << i << ", v = " << valuePerNode[i] << std::endl;
   }*/

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

      /*for(NodeId i = 0; i < inputPerNode.size(); ++i)
      {
         std::cout << "Input per node: n = " << i << ", i = " << inputPerNode[i] << std::endl;
      }*/

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

}