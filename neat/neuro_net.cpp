#include "neuro_net.hpp"
#include <cmath>
#include <iostream>
#include <algorithm>
#include "activation.hpp"

namespace neat
{

NeuroNet::NeuroNet(const neat::v2::Genom& genotype)
{
   mNodes.reserve(genotype.getNodeCount(v2::Genom::NodeType::All));
   mInputNodes.reserve(genotype.getNodeCount(v2::Genom::NodeType::Input));
   mOutputNodes.reserve(genotype.getNodeCount(v2::Genom::NodeType::Output));
   mHiddenNodes.reserve(genotype.getNodeCount(v2::Genom::NodeType::Hidden));

   std::map<NodeId, NodeId> idToIdxMap;
   for(auto iter = genotype.beginNodes(v2::Genom::NodeType::Bias); iter != genotype.endNodes(v2::Genom::NodeType::Bias); ++iter)
   {
      idToIdxMap[iter->id] = mNodes.size();
      mNodes.push_back(Node{static_cast<NodeId>(mNodes.size()), 1.0, -1});
   }

   for(auto iter = genotype.beginNodes(v2::Genom::NodeType::Input); iter != genotype.endNodes(v2::Genom::NodeType::Input); ++iter)
   {
      idToIdxMap[iter->id] = mNodes.size();
      mNodes.push_back(Node{static_cast<NodeId>(mNodes.size()), 0.0, 0});
      mInputNodes.push_back(&mNodes.back());
   }

   for(auto iter = genotype.beginNodes(v2::Genom::NodeType::Output); iter != genotype.endNodes(v2::Genom::NodeType::Output); ++iter)
   {
      idToIdxMap[iter->id] = mNodes.size();
      mNodes.push_back(Node{static_cast<NodeId>(mNodes.size()), 0.0, -1});
      mOutputNodes.push_back(&mNodes.back());
   }

   for(auto iter = genotype.beginNodes(v2::Genom::NodeType::Hidden); iter != genotype.endNodes(v2::Genom::NodeType::Hidden); ++iter)
   {
      idToIdxMap[iter->id] = mNodes.size();
      mNodes.push_back(Node{static_cast<NodeId>(mNodes.size()), 0.0, -1, {}, getPtr(iter->acType)});
      mHiddenNodes.push_back(&mNodes.back());
   }
   
   for(auto& c : genotype)
   {
      auto& src = mNodes[idToIdxMap[c.srcNodeId]];
      auto& dst = mNodes[idToIdxMap[c.dstNodeId]];

      dst.inputs.push_back({src.id, c.weight});

      if(src.id != dst.id && (src.depth <= dst.depth || dst.depth == -1))//Otherwise recursive - lets not adapt
      {
         int newDepth = src.depth + 1;
         dst.depth = std::max(newDepth, dst.depth);
      }
   }
   
   std::sort(mHiddenNodes.begin(), mHiddenNodes.end(), [&](auto x, auto y)
   {
      return x->depth < y->depth;
   });
}

void NeuroNet::activate()
{
   //Walk over ordered hidden nodes
   for(auto node : mHiddenNodes)
   {      
      double totalInput = 0;
      for(auto& c: node->inputs)
      {
         auto& input = mNodes[c.first];
         totalInput += c.second * input.value;
      }

      node->value = node->func(totalInput);
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

NetworkTopology NeuroNet::createTopology() const
{
   NetworkTopology result;

   int maxDepth = std::max_element(mNodes.begin(), mNodes.end(), [](auto x, auto y){return x.depth < y.depth;})->depth;

   for(auto &n: mNodes)
   {
      if(std::find_if(mOutputNodes.begin(), mOutputNodes.end(), [&](auto x) {return x->id == n.id;}) != mOutputNodes.end())
      {
         result.add(maxDepth, n.id, n.inputs);
      }
      else
      {
         result.add(std::min(maxDepth - 1, std::max(n.depth, 0)), n.id, n.inputs);
      }
   }

   return result;
}

void NetworkTopology::add(const std::size_t layerIndex, const NodeId id, boost::container::small_vector<std::pair<NodeId, double>, 10> inputs)
{
   mLayers[layerIndex].push_back({id, inputs});
}

const std::size_t NetworkTopology::getNumLayers() const
{
   return mLayers.size();
}

std::vector<NetworkTopology::Node> NetworkTopology::getLayer(const std::size_t index) const
{
   auto pos = mLayers.find(index);
   if(pos == mLayers.end())
   {
      return std::vector<NetworkTopology::Node>();
   }
   else
   {
      return pos->second;
   }
}

}