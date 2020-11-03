#include "neuro_net2.hpp"
#include <cmath>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "activation.hpp"
#include "../logger/Logger.hpp"

namespace neuroevolution
{

NeuroNet2::NeuroNet2(
   const std::vector<NodeId>& inputNodes, 
   const std::vector<NodeId>& outputNodes,
   const std::vector<HiddenNodeDef>& hiddenNodes,
   const std::vector<ConnectionDef>& connections
   )
{
   mNodes.reserve(inputNodes.size() + outputNodes.size() + hiddenNodes.size());
   mValues.resize(inputNodes.size() + outputNodes.size() + hiddenNodes.size(), 0);
   mInputNodes.reserve(inputNodes.size());
   mOutputNodes.reserve(outputNodes.size());
   mHiddenNodes.reserve(hiddenNodes.size());

   std::map<NodeId, NodeId> idToIdxMap;

   for(auto n : inputNodes)
   {
      idToIdxMap[n] = mNodes.size();
      mNodes.push_back(Node{static_cast<NodeId>(mNodes.size()), 0.0, 0});
      mInputNodes.push_back(&mNodes.back());
   }

   for(auto n : outputNodes)
   {
      idToIdxMap[n] = mNodes.size();
      mNodes.push_back(Node{static_cast<NodeId>(mNodes.size()), 0.0, -1});
      mOutputNodes.push_back(&mNodes.back());
   }

   for(auto n : hiddenNodes)
   {
      idToIdxMap[n.id] = mNodes.size();
      mValues[mNodes.size()] = n.bias;
      mNodes.push_back(Node{static_cast<NodeId>(mNodes.size()), n.bias, -1, {}, getPtr(n.acType), n.acType});
      mHiddenNodes.push_back(&mNodes.back());
   }
   
   for (auto& c : connections)
   {
      auto& src = mNodes[idToIdxMap[c.src]];
      auto& dst = mNodes[idToIdxMap[c.dst]];

      if (c.dst < mInputNodes.size())
      {
         throw - 1;
      }

      dst.inputs.push_back({src.id, c.weight});

      if(src.id != dst.id && (src.depth <= dst.depth || dst.depth == -1))//Otherwise recursive - lets not adapt
      {
         int newDepth = src.depth + 1;
         dst.depth = std::max(newDepth, dst.depth);
      }
   }

   mCons = Matrix(mNodes, connections.size());

   std::sort(mHiddenNodes.begin(), mHiddenNodes.end(), [&](auto x, auto y)
   {
      return x->depth < y->depth;
   });
}

void NeuroNet2::activate()
{
   //Walk over ordered hidden nodes
   for(auto node : mHiddenNodes)
   {
      double totalInput = 0;

      auto a = mCons.colInd[node->id];
      auto b = mCons.colInd[node->id + 1];
      for(std::uint16_t i = a; i < b; ++i)
      {
         totalInput += mValues[mCons.rowInd[i]] * mCons.weights[i];
      }

      mValues[node->id] = node->func(totalInput);
   }

   //Walk over ordered output nodes
   for(auto node : mOutputNodes)
   {      
      double totalInput = 0;

      for(std::uint16_t i = mCons.colInd[node->id]; i < mCons.colInd[node->id + 1]; ++i)
      {
         totalInput += mValues[mCons.rowInd[i]] * mCons.weights[i];
      }

      mValues[node->id] = totalInput;
   }
}  

NeuroNet2::NodeIterator NeuroNet2::begin_input()
{
   return mValues.begin();
}

NeuroNet2::NodeIterator NeuroNet2::end_input()
{
   return mValues.begin() + mInputNodes.size();
}

NeuroNet2::ConstNodeIterator NeuroNet2::begin_output() const
{
   return mValues.begin() + mInputNodes.size();
}

NeuroNet2::ConstNodeIterator NeuroNet2::end_output() const
{
   return begin_output() + mOutputNodes.size();
}

std::vector<double> activate(NeuroNet2& n, const std::vector<double>& input)
{
   std::vector<double> result;

   std::copy(input.begin(), input.end(), n.begin_input());

   n.activate();

   std::copy(n.begin_output(), n.end_output(), std::back_inserter(result));

   return result;
}

}