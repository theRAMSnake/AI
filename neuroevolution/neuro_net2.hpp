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
#include "neuro_net.hpp"
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/io.hpp>

using namespace boost::numeric::ublas;

namespace neuroevolution
{

class NeuroNet2
{
//private:
public:
   class Node;

   struct Matrix
   {
      std::vector<std::uint16_t> colInd;
      std::vector<std::uint16_t> rowInd;
      std::vector<double> weights;

      Matrix() = default;
   
      Matrix(const std::vector<Node>& mNodes, const std::size_t numConnections)
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

      void print()
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
   };
   
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
   
   NodeIterator begin_input();
   NodeIterator end_input();

   ConstNodeIterator begin_output() const;
   ConstNodeIterator end_output() const;

//private:
public:
   struct Node
   {
      NodeId id;
      double bias;
      int depth = -1;
      boost::container::small_vector<std::pair<NodeId, double>, 10> inputs;
      ActivationFunction func = nullptr;
      ActivationFunctionType accType;
   };

   std::vector<Node> mNodes;
   std::vector<double> mValues;

   std::vector<Node*> mHiddenNodes;
   std::vector<Node*> mInputNodes;
   std::vector<Node*> mOutputNodes;

   Matrix mCons;
};

std::vector<double> activate(NeuroNet2& n, const std::vector<double>& input);

}