#pragma once
#include "activation.hpp"
#include <vector>
#include <list>
#include <cstddef>
#include <unordered_map>
#include <boost/container/small_vector.hpp>
#include <map>
#include <memory>
#include "IPlayground.hpp"
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>

using namespace boost::numeric::ublas;

namespace neuroevolution
{

using NodeId = unsigned int;

class NetworkTopology
{
public:
   struct Node
   {
      NodeId id;
      boost::container::small_vector<std::pair<NodeId, double>, 10> inputs;
   };

   void add(const std::size_t layerIndex, const NodeId id, boost::container::small_vector<std::pair<NodeId, double>, 10> inputs);

   const std::size_t getNumLayers() const;
   std::vector<Node> getLayer(const std::size_t index) const;

private:
   std::map<std::size_t, std::vector<Node>> mLayers;
};

class NeuroNet2
{  
public:
   using NodeIterator = std::vector<double>::iterator;
   using ConstNodeIterator = std::vector<double>::const_iterator;

   struct ConnectionDef
   {
      NodeId src;
      NodeId dst;
      double weight;
   };

   struct HiddenNodeDef
   {
      NodeId id;
      ActivationFunctionType acType;
      double bias;
   };

   NeuroNet2(
      const std::vector<NodeId>& inputNodes, 
      const std::vector<NodeId>& outputNodes,
      const std::vector<HiddenNodeDef>& hiddenNodes,
      const std::vector<ConnectionDef>& connections
      );

   void activate();
   void reset();

   NetworkTopology createTopology() const;
   
   NodeIterator begin_input();
   NodeIterator end_input();

   ConstNodeIterator begin_output() const;
   ConstNodeIterator end_output() const;

   static std::unique_ptr<NeuroNet2> fromBinaryStream(std::ifstream& stream);
   void toBinaryStream(std::ofstream& stream);

#ifndef TEST
private:
#endif
   NeuroNet2() = default;
   struct Node
   {
      NodeId id;
      double bias;
      int depth = -1;
      boost::container::small_vector<std::pair<NodeId, double>, 10> inputs;
      ActivationFunction func = nullptr;
      ActivationFunctionType accType;
   };

   struct Matrix
   {
      std::vector<std::uint16_t> colInd;
      std::vector<std::uint16_t> rowInd;
      std::vector<double> weights;

      Matrix() = default;
   
      Matrix(const std::vector<Node>& mNodes, const std::size_t numConnections);
      void print();
   };

   std::vector<Node> mNodes;
   std::vector<double> mValues;

   std::vector<Node*> mHiddenNodes;
   std::vector<Node*> mInputNodes;
   std::vector<Node*> mOutputNodes;

   Matrix mCons;
};

std::vector<double> activate(NeuroNet2& n, const std::vector<double>& input);

class NNAgent : public IAgent
{
public:
   NNAgent(const unsigned int numInputs, const unsigned int numOutputs, std::unique_ptr<NeuroNet2>&& nn);

   void reset() override;
   unsigned int run(const double* input) override;
   void toBinaryStream(std::ofstream& stream) const override;

   NeuroNet2& getNN();

private:
   const unsigned int mNumInputs;
   const unsigned int mNumOutputs;
   std::unique_ptr<NeuroNet2> mNn;
};

}