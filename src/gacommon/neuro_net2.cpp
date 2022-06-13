#include "neuro_net2.hpp"
#include <cmath>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "activation.hpp"
#include "../logger/Logger.hpp"
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

namespace gacommon
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

NetworkTopology NeuroNet2::createTopology() const
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

void NetworkTopology::add(const std::size_t layerIndex, const NodeId id, const std::vector<std::pair<NodeId, double>>& inputs)
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

NNAgent::NNAgent(const unsigned int numInputs, const unsigned int numOutputs, std::unique_ptr<NeuroNet2>&& nn)
: mNumInputs(numInputs)
, mNumOutputs(numOutputs)
, mNn(std::move(nn))
{

}

void NNAgent::reset()
{
   mNn->reset();
}

void NNAgent::run(const std::vector<IOElement>& inputs, std::vector<IOElement>& output)
{
   auto iIter = mNn->begin_input();
   for(auto& i : inputs)
   {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr(std::is_same_v<ValueIO, T>)
            {
                *iIter = arg.value;
                ++iIter;
            }
            else if constexpr(std::is_same_v<ChoiceIO, T>)
            {
                for(std::size_t j = 0; j < arg.options; ++j)
                {
                    *(iIter + j) = 0;
                } 
                *(iIter + arg.selection) = 1;
                iIter += arg.options;
            }
            else if constexpr(std::is_same_v<BitmapIO, T>)
            {
                for(auto b : arg.map)
                {
                    *iIter = static_cast<double>(b);
                    ++iIter;
                }
            }
            else
            {
                throw std::runtime_error("Unhandled in run");
            }
        }, i);
   }

   mNn->activate();

   auto oIter = mNn->begin_output();
   for(auto& o : output)
   {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr(std::is_same_v<ValueIO, T>)
            {
                arg.value = *oIter;
                ++oIter;
            }
            else if constexpr(std::is_same_v<ChoiceIO, T>)
            {
                auto pos = std::max_element(oIter, oIter + arg.options);
                arg.selection = std::distance(oIter, pos);
                oIter += arg.options;
            }
            else if constexpr(std::is_same_v<BitmapIO, T>)
            {
                for(auto& b : arg.map)
                {
                    b = static_cast<std::uint8_t>(*oIter);
                    ++oIter;
                }
            }
            else
            {
                throw std::runtime_error("Unhandled in run");
            }
        }, o);
   }
}

void NNAgent::toBinaryStream(std::ofstream& stream) const
{
   mNn->toBinaryStream(stream);
}

NeuroNet2& NNAgent::getNN()
{
   return *mNn;
}

void NeuroNet2::reset()
{
   std::fill(mValues.begin(), mValues.end(), 0);
}

NeuroNet2::Matrix::Matrix(const std::vector<Node>& mNodes, const std::size_t numConnections)
{
   colInd.resize(mNodes.size() + 1);
   weights.resize(numConnections);
   rowInd.resize(numConnections);

   colInd[0] = 0;
   std::size_t rowIndex = 0;
   for(std::size_t i = 0; i < mNodes.size(); ++i)
   {
      auto& n = mNodes[i];

      for(auto& c : n.inputs)
      {
         rowInd[rowIndex] = c.first;
         weights[rowIndex] = c.second;

         rowIndex++;
      }

      colInd[i + 1] = colInd[i] + n.inputs.size();
   }
}

void NeuroNet2::Matrix::print()
{
   std::cout << "COLS: [";
   for(auto i : colInd)
   {
      std::cout << i << " ";
   }
   std::cout << "]\n";
   std::cout << "ROWS: [";
   for(auto i : rowInd)
   {
      std::cout << i << " ";
   }
   std::cout << "]\n";
   std::cout << "WEIGHTS: [";
   for(auto i : weights)
   {
      std::cout << i << " ";
   }
   std::cout << "]\n";
}

std::unique_ptr<NeuroNet2> NeuroNet2::fromBinaryStream(std::ifstream& stream)
{
   auto result = new NeuroNet2();
   boost::archive::binary_iarchive ia(stream);

   std::vector<NodeId> inputs;
   std::vector<NodeId> outputs;
   std::vector<NodeId> hiddens;

   ia >> result->mNodes >> result->mValues >> hiddens >> inputs >> outputs;

   std::size_t numConns = 0;
   std::size_t i = 0;
   std::map<NodeId, NodeId> idToIdxMap;
   for(auto& n : result->mNodes)
   {
       n.func = getPtr(n.accType);
       numConns += n.inputs.size();
       idToIdxMap[n.id] = i++;
   }
   for(auto i : inputs)
   {
       result->mInputNodes.push_back(&result->mNodes[idToIdxMap[i]]);
   }
   for(auto i : outputs)
   {
       result->mOutputNodes.push_back(&result->mNodes[idToIdxMap[i]]);
   }
   for(auto i : hiddens)
   {
       result->mHiddenNodes.push_back(&result->mNodes[idToIdxMap[i]]);
   }

   result->mCons = Matrix(result->mNodes, numConns);

   return std::unique_ptr<NeuroNet2>{result};
}

void NeuroNet2::toBinaryStream(std::ofstream& stream)
{
    auto toIdVector = [](const std::vector<Node*>& input)
    {
        std::vector<NodeId> result(input.size());
        std::transform(input.begin(), input.end(), result.begin(), [](auto x){return x->id;});
        return result;
    };

    boost::archive::binary_oarchive oa(stream);
    oa << mNodes;
    oa << mValues;
    oa << toIdVector(mHiddenNodes);
    oa << toIdVector(mInputNodes);
    oa << toIdVector(mOutputNodes);
}

}
