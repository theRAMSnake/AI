#include "neuro_net.hpp"
#include <cmath>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "activation.hpp"
#include "../logger/Logger.hpp"

namespace neuroevolution
{

void dumpConnections(const std::vector<NeuroNet::ConnectionDef>& connections)
{
    for (auto& c : connections)
    {
        LOG("C: " + std::to_string(c.src) + "-" + std::to_string(c.dst));
    }
}

NeuroNet::NeuroNet(
   const std::vector<NodeId>& inputNodes, 
   const std::vector<NodeId>& outputNodes,
   const std::vector<HiddenNodeDef>& hiddenNodes,
   const std::vector<ConnectionDef>& connections
   )
{
   mNodes.reserve(inputNodes.size() + outputNodes.size() + hiddenNodes.size());
   mInputNodes.reserve(inputNodes.size());
   mOutputNodes.reserve(outputNodes.size());
   mHiddenNodes.reserve(hiddenNodes.size());

   std::map<NodeId, NodeId> idToIdxMap;

   for(auto n : inputNodes)
   {
      idToIdxMap[n] = mNodes.size();
      mNodes.push_back(Node{static_cast<NodeId>(mNodes.size()), 0.0, 0.0, 0});
      mInputNodes.push_back(&mNodes.back());
   }

   for(auto n : outputNodes)
   {
      idToIdxMap[n] = mNodes.size();
      mNodes.push_back(Node{static_cast<NodeId>(mNodes.size()), 0.0, 0.0, -1});
      mOutputNodes.push_back(&mNodes.back());
   }

   for(auto n : hiddenNodes)
   {
      idToIdxMap[n.id] = mNodes.size();
      mNodes.push_back(Node{static_cast<NodeId>(mNodes.size()), n.bias, n.bias, -1, {}, getPtr(n.acType), n.acType});
      mHiddenNodes.push_back(&mNodes.back());
   }
   
   for (auto& c : connections)
   {
       auto& src = mNodes[idToIdxMap[c.src]];
       auto& dst = mNodes[idToIdxMap[c.dst]];

       if (c.dst < mInputNodes.size())
       {
           dumpConnections(connections);
           throw - 1;
       }

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

void NeuroNet::reset()
{
   for(auto node : mHiddenNodes)
   {      
      node->value = node->bias;
   }

   for(auto node : mOutputNodes)
   {      
      node->value = node->bias;
   }
}  

std::vector<NodeId> readIdList(std::ifstream& stream)
{
   std::vector<NodeId> result;
   std::size_t size = 0;
   stream.read((char*)&size, sizeof(std::size_t));
   for(std::size_t i = 0; i < size; ++i)
   {
      NodeId id;
      stream.read((char*)&id, sizeof(NodeId));

      result.push_back(id);
   }

   return result;
}

std::unique_ptr<NeuroNet> NeuroNet::fromBinaryStream(std::ifstream& stream)
{
   auto result = new NeuroNet();

   std::vector<NodeId> inputs = readIdList(stream);
   std::vector<NodeId> outputs = readIdList(stream);
   std::vector<NodeId> hiddens = readIdList(stream);

   //Reading nodes
   std::size_t size = 0;
   stream.read((char*)&size, sizeof(std::size_t));

   result->mNodes.reserve(size);

   for(std::size_t i = 0; i < size; ++i)
   {
      NodeId id;
      stream.read((char*)&id, sizeof(NodeId));

      double bias = 0.0;
      stream.read((char*)&bias, sizeof(double));

      int depth = 0;
      stream.read((char*)&depth, sizeof(int));

      std::size_t numInputs = 0;
      stream.read((char*)&numInputs, sizeof(std::size_t));

      boost::container::small_vector<std::pair<NodeId, double>, 10> links;
      for(std::size_t j = 0; j < numInputs; j++)
      {
         NodeId src;
         stream.read((char*)&src, sizeof(NodeId));

         double weight;
         stream.read((char*)&weight, sizeof(double));

         links.push_back({src, weight});
      }

      ActivationFunctionType actType;
      stream.read((char*)&actType, sizeof(ActivationFunctionType));

      //Process node
      if(std::find(inputs.begin(), inputs.end(), id) != inputs.end())
      {
         result->mNodes.push_back(Node{id, 0.0, bias, depth});
         result->mInputNodes.push_back(&result->mNodes.back());
      }
      else if(std::find(outputs.begin(), outputs.end(), id) != outputs.end())
      {
         result->mNodes.push_back(Node{id, 0.0, bias, depth, links});
         result->mOutputNodes.push_back(&result->mNodes.back());
      }
      else if(std::find(hiddens.begin(), hiddens.end(), id) != hiddens.end())
      {
         result->mNodes.push_back(Node{id, 0.0, bias, depth, links, getPtr(actType), actType});
         result->mHiddenNodes.push_back(&result->mNodes.back());
      }
      else
      {
         result->mNodes.push_back(Node{id, 1.0, bias, depth});
      }
   }

   std::sort(result->mHiddenNodes.begin(), result->mHiddenNodes.end(), [&](auto x, auto y)
   {
      return x->depth < y->depth;
   });

   return std::unique_ptr<NeuroNet>(result);
}

void NeuroNet::toBinaryStream(std::ofstream& stream)
{
   //Input ids
   auto size = mInputNodes.size();
   stream.write((char*)&size, sizeof(std::size_t));
   for(auto& n : mInputNodes)
   {
      stream.write((char*)&n->id, sizeof(NodeId));
   }
   
   //Output ids
   size = mOutputNodes.size();
   stream.write((char*)&size, sizeof(std::size_t));
   for(auto& n : mOutputNodes)
   {
      stream.write((char*)&n->id, sizeof(NodeId));
   }

   //Hidden ids
   size = mHiddenNodes.size();
   stream.write((char*)&size, sizeof(std::size_t));
   for(auto& n : mHiddenNodes)
   {
      stream.write((char*)&n->id, sizeof(NodeId));
   }

   //Node defs
   size = mNodes.size();
   stream.write((char*)&size, sizeof(std::size_t));
   for(auto& n : mNodes)
   {
      stream.write((char*)&n.id, sizeof(NodeId));
      //Skip value as irrelevant
      stream.write((char*)&n.bias, sizeof(double));
      stream.write((char*)&n.depth, sizeof(int));
      size = n.inputs.size();
      stream.write((char*)&size, sizeof(std::size_t));

      for(auto& i : n.inputs)
      {
         stream.write((char*)&i.first, sizeof(NodeId));
         stream.write((char*)&i.second, sizeof(double));
      }

      auto actType = n.accType;
      stream.write((char*)&actType, sizeof(ActivationFunctionType));
   }
}

NeuroNet::NeuroNet()
{

}

void NeuroNet::print()
{
   std::cout << std::endl;
   for(auto& n : mNodes)
   {
      std::cout << n.id << ' ' << static_cast<int>(n.accType) << ' ' << n.depth << ' ' << reinterpret_cast<long long int>(n.func) << ' ' << n.value << " [" ;
         
      for(auto& i : n.inputs)
      {
         std::cout << i.first << ' ' << i.second << ';';
      }

      std::cout << ']' << std::endl;
   }

   for(auto& x : mHiddenNodes)
   {
      std::cout << x->id << ' ';
   }

   std::cout << std::endl;

   for(auto& x : mOutputNodes)
   {
      std::cout << x->id << ' ';
   }

   std::cout << std::endl;

   for(auto& x : mInputNodes)
   {
      std::cout << x->id << ' ';
   }

   std::cout << std::endl;
}

NNAgent::NNAgent(const unsigned int numInputs, const unsigned int numOutputs, std::unique_ptr<NeuroNet>&& nn)
: mNumInputs(numInputs)
, mNumOutputs(numOutputs)
, mNn(std::move(nn))
{

}

void NNAgent::reset()
{
   mNn->reset();
}

unsigned int NNAgent::run(const double* input)
{
   auto oIter = mNn->begin_input();
   auto iIter = input;
   for(unsigned int i = 0; i < mNumInputs; ++i)
   {
      *oIter = *iIter;
      ++oIter;
      ++iIter;
   }

   mNn->activate();

   auto pos = std::max_element(mNn->begin_output(), mNn->end_output());

   return std::distance(mNn->begin_output(), pos);
}

void NNAgent::toBinaryStream(std::ofstream& stream) const
{
   mNn->toBinaryStream(stream);
}

NeuroNet& NNAgent::getNN()
{
   return *mNn;
}

}