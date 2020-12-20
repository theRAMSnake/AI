#pragma once
#include <vector>
#include <set>
#include <variant>
#include "neuroevolution/IPlayground.hpp"
#include "neuroevolution/activation.hpp"
#include "neuroevolution/BinaryIO.hpp"

namespace snakega
{

class Genom
{
public:
   friend class GenomDecoder;
   friend class SnakeGATest;
   Genom(const std::size_t numInputs, const std::size_t numOutputs);

   void operator= (const Genom& other);
   bool operator==(const Genom& other) const;

   static Genom createMinimal(const std::size_t numInputs, const std::size_t numOutputs);

   void mutateStructure();
   void mutateParameters();
   void crossoverParametersFrom(const Genom& other);

   std::size_t getNumNeurons() const;
   unsigned int getComplexity() const;
   unsigned int getNumInputs() const;
   unsigned int getNumOutputs() const;

   static void loadState(neuroevolution::BinaryInput& in, Genom& g);
   void saveState(neuroevolution::BinaryOutput& out) const;

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

      bool operator == (const ExtConnectionDef& other) const
      {
         return srcNodeId == other.srcNodeId && dstNodeId == other.dstNodeId && weight == other.weight;
      }
   };

   struct IntConnectionDef
   {
      LocalNodeId srcNodeId;
      LocalNodeId dstNodeId;
      double weight;

      bool operator == (const IntConnectionDef& other) const
      {
         return srcNodeId == other.srcNodeId && dstNodeId == other.dstNodeId && weight == other.weight;
      }
   };

   struct LocalNeuronDef
   {
      LocalNodeId id;
      ActivationFunctionType acType;

      bool operator == (const LocalNeuronDef& other) const
      {
         return id == other.id && acType == other.acType;
      }
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

      bool operator == (const NeuroBlockDefinition& other) const
      {
         return id == other.id && neurons == other.neurons && internalConnections == other.internalConnections;
      }
   };

   struct NeuroBlock
   {
      NeuroBlockId blockId;
      NeuroBlockId parentBlockId;
      NeuroBlockDefinitionId definitionId;
      std::vector<ExtConnectionDef> externalConnections;

      bool operator == (const NeuroBlock& other) const
      {
         return blockId == other.blockId && parentBlockId == other.parentBlockId && 
            definitionId == other.definitionId && externalConnections == other.externalConnections;
      }
   };

#ifndef TEST
private:
#endif

   bool mutateNewDefinition();
   void mutateRemoveLeafBlock();
   void mutateNewNeuron();
   void mutateNewConnection();

   std::size_t mNumInputs;
   std::size_t mNumOutputs;

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
   std::vector<NeuroBlockId> getLeafBlocks() const;
   std::vector<GlobalNodeId> getDisconnectedNeuronIds() const;

   std::vector<NeuroBlockDefinition> mDefs;
   std::vector<NeuroBlock> mBlocks;
};

neuroevolution::BinaryOutput& operator << (neuroevolution::BinaryOutput& out, const Genom& g);
neuroevolution::BinaryInput& operator >> (neuroevolution::BinaryInput& in, Genom& g);

neuroevolution::BinaryOutput& operator << (neuroevolution::BinaryOutput& out, const Genom::NeuroBlockDefinition& g);
neuroevolution::BinaryInput& operator >> (neuroevolution::BinaryInput& in, Genom::NeuroBlockDefinition& g);

neuroevolution::BinaryOutput& operator << (neuroevolution::BinaryOutput& out, const Genom::NeuroBlock& g);
neuroevolution::BinaryInput& operator >> (neuroevolution::BinaryInput& in, Genom::NeuroBlock& g);

neuroevolution::BinaryOutput& operator << (neuroevolution::BinaryOutput& out, const Genom::IntConnectionDef& g);
neuroevolution::BinaryInput& operator >> (neuroevolution::BinaryInput& in, Genom::IntConnectionDef& g);

neuroevolution::BinaryOutput& operator << (neuroevolution::BinaryOutput& out, const Genom::ExtConnectionDef& g);
neuroevolution::BinaryInput& operator >> (neuroevolution::BinaryInput& in, Genom::ExtConnectionDef& g);

neuroevolution::BinaryOutput& operator << (neuroevolution::BinaryOutput& out, const Genom::GlobalNodeId& g);
neuroevolution::BinaryInput& operator >> (neuroevolution::BinaryInput& in, Genom::GlobalNodeId& g);

neuroevolution::BinaryOutput& operator << (neuroevolution::BinaryOutput& out, const Genom::LocalNeuronDef& g);
neuroevolution::BinaryInput& operator >> (neuroevolution::BinaryInput& in, Genom::LocalNeuronDef& g);

}