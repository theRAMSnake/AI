#pragma once
#include "activation.hpp"
#include <vector>
#include <list>
#include <cstddef>
#include <unordered_map>
#include <boost/container/small_vector.hpp>
#include <map>

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

class NeuroNet
{
private:
   class Node;
   
public:
   class NodeIterator
   {
   public:
      using difference_type = std::ptrdiff_t;
      using value_type = double;
      using pointer = double*;
      using reference = double&;
      using iterator_category = std::random_access_iterator_tag;

      NodeIterator(Node** nodePtr);

      inline reference operator* ()
      {
         return (*mNodePtr)->value;
      }

      inline const reference operator* () const
      {
         return (*mNodePtr)->value;
      }

      inline NodeIterator& operator++ ()
      {
         mNodePtr++;
         return *this;
      }
      
      bool operator== (const NodeIterator& other) const;
      bool operator!= (const NodeIterator& other) const;
      difference_type operator- (const NodeIterator& other) const;

   private:
      Node** mNodePtr;
   };

   struct ConnectionDef
   {
      NodeId src;
      NodeId dst;
      double weight;
   };

   NeuroNet(
      const std::vector<NodeId>& inputNodes, 
      const std::vector<NodeId>& biasNodes,
      const std::vector<NodeId>& outputNodes,
      const std::vector<std::pair<NodeId, ActivationFunctionType>>& hiddenNodes,
      const std::vector<ConnectionDef>& connections
      );

   void activate();
   void reset();
   
   NodeIterator begin_input();
   NodeIterator end_input();

   const NodeIterator begin_output() const;
   const NodeIterator end_output() const;

   NetworkTopology createTopology() const;

   static NeuroNet fromBinaryStream(std::ifstream& stream);

private:
   struct Node
   {
      NodeId id;
      double value;
      int depth = -1;
      boost::container::small_vector<std::pair<NodeId, double>, 10> inputs;
      ActivationFunction func = nullptr;
   };

   std::vector<Node> mNodes;

   std::vector<Node*> mHiddenNodes;
   std::vector<Node*> mInputNodes;
   std::vector<Node*> mOutputNodes;
};

std::vector<double> activate(NeuroNet& n, const std::vector<double>& input);

}