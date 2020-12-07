#pragma once
#include <vector>
#include <set>
#include <variant>
#include "neuroevolution/IPlayground.hpp"
#include "neuroevolution/activation.hpp"

namespace snakega
{

class Genom
{
public:
   friend class GenomDecoder;
   friend class SnakeGATest;
   Genom(const std::size_t numInputs, const std::size_t numOutputs);

   void operator= (const Genom& other);

   static Genom createMinimal(const std::size_t numInputs, const std::size_t numOutputs);

   void mutateStructure();
   void mutateParameters();
   void crossoverParametersFrom(const Genom& other);

   std::size_t getNumNeurons() const;
   unsigned int getComplexity() const;
   unsigned int getNumInputs() const;
   unsigned int getNumOutputs() const;

   static Genom loadState(std::ifstream& s, const std::size_t numInputs, const std::size_t numOutputs);
   void saveState(std::ofstream& s) const;

#ifndef TEST
private:
#endif

   const std::size_t mNumInputs;
   const std::size_t mNumOutputs;

   using NeuroBlockDefinitionId = std::size_t;
   const NeuroBlockDefinitionId ROOT_ID = 0;
   const NeuroBlockDefinitionId INVALID_ID = std::numeric_limits<std::size_t>::max();
   using NeuroBlockId = std::size_t;
   using LocalNodeId = std::size_t;

   enum class GlobalNodeType : unsigned int
   {
      Input,
      Output,
      Local
   };

   struct GlobalNodeId
   {
      GlobalNodeType type : 2;
      NeuroBlockId blockId : 14; //Local only
      LocalNodeId localId : 16;

      GlobalNodeId() = default;
      GlobalNodeId(const GlobalNodeType _type, const NeuroBlockId _blockId, const LocalNodeId _localId);

      bool operator == (const GlobalNodeId& other) const;
   };

   struct ExtConnectionDef
   {
      GlobalNodeId srcNodeId;
      GlobalNodeId dstNodeId;
      double weight;
   };

   struct IntConnectionDef
   {
      LocalNodeId srcNodeId;
      LocalNodeId dstNodeId;
      double weight;
   };

   struct LocalNeuronDef
   {
      LocalNodeId id;
      ActivationFunctionType acType;
   };

   struct GlobalNeuronDef
   {
      GlobalNodeId id;
      ActivationFunctionType acType;
   };

   struct NeuroBlockDefinition
   {
      NeuroBlockDefinitionId id;
      std::vector<LocalNeuronDef> neurons;
      std::vector<IntConnectionDef> internalConnections;
   };

   struct NeuroBlock
   {
      NeuroBlockId blockId;
      NeuroBlockId parentBlockId;
      NeuroBlockDefinitionId definitionId;
      std::vector<ExtConnectionDef> externalConnections;
   };

   class ConnectionsIterator
   {
   public:
      ConnectionsIterator(Genom& genom, const bool isEnd);

      bool operator != (const ConnectionsIterator& other) const;
      bool operator == (const ConnectionsIterator& other) const;
      ConnectionsIterator& operator ++();
      const ExtConnectionDef* operator -> () const;
      const ExtConnectionDef& operator *() const;

      bool isExternal() const;

      using difference_type = std::ptrdiff_t;
      using value_type = ExtConnectionDef;
      using pointer = ExtConnectionDef *;
      using reference = ExtConnectionDef &;
      using iterator_category = std::forward_iterator_tag;

      void initFromBlock();
      void updateValue() const;
      void findSuitableBlock();

      Genom* mGenom;
      std::vector<NeuroBlock>::iterator mBlockIter;
      std::vector<NeuroBlockDefinition>::iterator mDefIter;
      std::vector<IntConnectionDef>::iterator mInternalIter;
      std::vector<ExtConnectionDef>::iterator mExternalIter;
      mutable ExtConnectionDef mValue; 
   };
   friend class ConnectionsIterator;
   ConnectionsIterator beginConnections() const;
   ConnectionsIterator endConnections() const;

   class NodesIterator
   {
   public:
      NodesIterator(const Genom& genom, const bool isEnd);

      bool operator != (const NodesIterator& other) const;
      bool operator == (const NodesIterator& other) const;
      NodesIterator& operator ++();
      const GlobalNeuronDef* operator -> () const;
      const GlobalNeuronDef& operator *() const;

      using difference_type = std::ptrdiff_t;
      using value_type = GlobalNeuronDef;
      using pointer = GlobalNeuronDef *;
      using reference = GlobalNeuronDef &;
      using iterator_category = std::forward_iterator_tag;

      void updateValue() const;
      void findSuitableBlock();

      const Genom* mGenom;
      std::vector<NeuroBlock>::const_iterator mBlockIter;
      std::vector<NeuroBlockDefinition>::const_iterator mDefIter;
      std::vector<LocalNeuronDef>::const_iterator mNodeIter;
      mutable GlobalNeuronDef mValue; 
   };
   friend class NodesIterator;
   NodesIterator beginNodes() const;
   NodesIterator endNodes() const;

   const NeuroBlockDefinition& createDefinition();
   std::vector<NeuroBlock>::iterator createBlock(const NeuroBlockDefinition& def, const NeuroBlockId& parentId);
   void erase(ConnectionsIterator iter);
   void erase(const GlobalNodeId id);
   void updateWeight(ConnectionsIterator iter, const double newWeight);
   GlobalNodeId getRandomSource() const;
   GlobalNodeId getRandomTarget() const;

   void writeBlock(const NeuroBlock& block, std::ofstream& s) const;
   static void loadBlock(NeuroBlock& block, std::ifstream& s);

   std::vector<NeuroBlockDefinition> mDefs;
   std::vector<NeuroBlock> mBlocks;
};

}