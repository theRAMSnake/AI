#pragma once
#include "activation.hpp"
#include <vector>
#include <list>
#include <cstddef>
#include <unordered_map>
#include <map>
#include <memory>
#include "IPlayground.hpp"
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/serialization/vector.hpp>

using namespace boost::numeric::ublas;

namespace gacommon
{

using NodeId = unsigned int;

class NetworkTopology
{
public:
   struct Node
   {
      NodeId id;
      std::vector<std::pair<NodeId, double>> inputs;
   };

   void add(const std::size_t layerIndex, const NodeId id, const std::vector<std::pair<NodeId, double>>& inputs);

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
      //boost::container::small_vector<std::pair<NodeId, double>, 10> inputs;
      std::vector<std::pair<NodeId, double>> inputs;
      ActivationFunction func = nullptr;
      ActivationFunctionType accType;

      friend class boost::serialization::access;
      template <class Archive>
      void serialize(Archive &ar, const unsigned int version)
      {
          ar & id & bias & depth & inputs & accType;
      }
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
   void run(const std::vector<IOElement>& inputs, std::vector<IOElement>& output) override;
   void toBinaryStream(std::ofstream& stream) const override;

   NeuroNet2& getNN();

private:
   std::unique_ptr<NeuroNet2> mNn;
};

}
