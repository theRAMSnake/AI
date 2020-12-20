#include "genom.hpp"
#include "neuroevolution/rng.hpp"
#include <fstream>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <numeric>
#include "logger/Logger.hpp"

namespace std
{

template<class Vec, class Pred>
void erase_if(Vec& c, Pred pred)
{
   auto it = std::remove_if(c.begin(), c.end(), pred);
   c.erase(it, c.end());
}

}

namespace snakega
{

Genom::Genom(const std::size_t numInputs, const std::size_t numOutputs)
: mNumInputs(numInputs)
, mNumOutputs(numOutputs)
{
   mBlocks.push_back({ROOT_ID, INVALID_ID, ROOT_ID});
   mDefs.push_back({ROOT_ID});
}

bool Genom::mutateNewDefinition()
{
   const double NEW_DEFINITION_CHANCE = 0.2;

   auto numCons = getComplexity();

   auto conn = std::next(beginConnections(), Rng::genChoise(numCons));
   auto from = conn->srcNodeId;
   auto to = conn->dstNodeId;
   auto parentId = conn.mBlockIter->blockId;

   if(conn.isExternal())
   {
      erase(conn);

      if(mDefs.size() == 1 || Rng::genProbability(NEW_DEFINITION_CHANCE))
      {
         //Gen new def and instantiate
         auto& def = createDefinition();
         auto pos = createBlock(def, parentId);

         auto newBlockNodeId = GlobalNodeId{GlobalNodeType::Local, pos->blockId, 0};
         pos->externalConnections.push_back({from, newBlockNodeId, Rng::genWeight()});
         pos->externalConnections.push_back({newBlockNodeId, to, Rng::genWeight()});
      }
      else
      {
         //just instantiate
         auto& def = *std::next(mDefs.begin() + 1, Rng::genChoise(mDefs.size() - 1));
         auto pos = createBlock(def, parentId);

         if(def.neurons.size() == 0)
         {
            def.neurons.push_back({0, static_cast<ActivationFunctionType>(Rng::genChoise(NUM_ACTIVATION_FUNCTION_TYPES))});
         }

         auto nodeId = GlobalNodeId{
            GlobalNodeType::Local, 
            pos->blockId, 
            std::next(def.neurons.begin(), Rng::genChoise(def.neurons.size()))->id
         };

         pos->externalConnections.push_back({from, nodeId, Rng::genWeight()});
         pos->externalConnections.push_back({nodeId, to, Rng::genWeight()});
      }

      return true;
   }

   return false;
}

std::vector<Genom::NeuroBlockId> Genom::getLeafBlocks() const
{
   std::vector<Genom::NeuroBlockId> result;

   for(std::size_t i = 1; i < mBlocks.size() - 1; ++i)
   {
      if(mBlocks[i+1].parentBlockId != mBlocks[i].blockId)
      {
         result.push_back(mBlocks[i].blockId);
      }
   }

   if(mBlocks.size() > 1)
   {
      result.push_back(mBlocks.back().blockId);
   }
   

   return result;
}

void Genom::mutateRemoveLeafBlock()
{
   auto leafs = getLeafBlocks();

   if(leafs.empty())
   {
      return;
   }

   auto randomLeafId = *std::next(leafs.begin(), Rng::genChoise(leafs.size()));

   auto blockIter = std::find_if(mBlocks.begin(), mBlocks.end(), [randomLeafId](auto x){return x.blockId == randomLeafId;});
   auto defId = blockIter->definitionId;

   mBlocks.erase(blockIter);

   //If there is not more blocks with this definition - remove the definition
   auto anotherBlockWithSameDefIdIter = std::find_if(mBlocks.begin(), mBlocks.end(), [defId](auto x){return x.definitionId == defId;});
   if(anotherBlockWithSameDefIdIter == mBlocks.end())
   {
      std::erase_if(mDefs, [defId](auto x){return x.id == defId;});
   }
}

void Genom::mutateNewNeuron()
{
   auto& d = *std::next(mDefs.begin(), Rng::genChoise(mDefs.size()));

   if(d.neurons.empty())
   {
      LocalNodeId newId = 1;
      d.neurons.push_back({newId, static_cast<ActivationFunctionType>(Rng::genChoise(NUM_ACTIVATION_FUNCTION_TYPES))});
   }
   else
   {
      auto n1 = std::next(d.neurons.begin(), Rng::genChoise(d.neurons.size()));
      auto n2 = std::next(d.neurons.begin(), Rng::genChoise(d.neurons.size()));

      auto newId = std::max_element(d.neurons.begin(), d.neurons.end(), [](auto x, auto y){return x.id < y.id;})->id + 1;
      d.neurons.push_back({newId, static_cast<ActivationFunctionType>(Rng::genChoise(NUM_ACTIVATION_FUNCTION_TYPES))});
      
      d.internalConnections.push_back({n1->id, newId, Rng::genWeight()});
      d.internalConnections.push_back({newId, n2->id, Rng::genWeight()});
   }
}

std::vector<Genom::GlobalNodeId> Genom::getDisconnectedNeuronIds() const
{
   std::vector<GlobalNodeId> disconnectedNeuronIds;
   for(auto iter = beginNodes(); iter != endNodes(); ++iter)
   {
      auto id = iter->id;
      if(endConnections() == std::find_if(beginConnections(), endConnections(), [id](auto x){return x.srcNodeId == id || x.dstNodeId == id;}))
      {
         disconnectedNeuronIds.push_back(id);
      }
   }

   return disconnectedNeuronIds;
}

void Genom::mutateNewConnection()
{
   auto src = getRandomSource();
   auto dst = getRandomTarget();
   
   auto srcBlockId = src.type == GlobalNodeType::Local ? src.blockId : mBlocks.begin()->blockId;
   auto& srcBlock = *std::find_if(mBlocks.begin(), mBlocks.end(), [srcBlockId](auto x){return x.blockId == srcBlockId;});

   auto dstBlockId = dst.type == GlobalNodeType::Local ? dst.blockId : mBlocks.begin()->blockId;
   auto& dstBlock = *std::find_if(mBlocks.begin(), mBlocks.end(), [dstBlockId](auto x){return x.blockId == dstBlockId;});

   if(srcBlock.definitionId == dstBlock.definitionId)
   {
      auto& def = *std::find_if(mDefs.begin(), mDefs.end(), [&srcBlock](auto x){return x.id == srcBlock.definitionId;});
      def.internalConnections.push_back({src.localId, dst.localId, Rng::genWeight()});
   }
   else
   {
      srcBlock.externalConnections.push_back({src, dst, Rng::genWeight()});
   }
}

void Genom::mutateStructure()
{
   const double CONNECTION_TO_BLOCK_CHANCE = 0.005;
   const double LEAF_BLOCK_REMOVE_CHANCE = 0.02;
   const double NEW_NEURON_IN_DEFINITION_CHANCE = 0.03;
   const double REMOVE_UNCONNECTED_NEURON_CHANCE = 0.1;
   const double CONNECT = 0.005;
   const double DISCONNECT = 0.005;
   
   //1. Turn a connection to a block (Either new def or copy def)
   auto numCons = getComplexity();
   auto numMutations = Rng::genProbabilities(CONNECTION_TO_BLOCK_CHANCE, numCons);
   for(unsigned int i = 0; i < numMutations; ++i)
   {
      mutateNewDefinition();
   }
   
   //2. Remove leaf block (and clean up definition possibly)
   auto leafBlocks = getLeafBlocks();
   numMutations = Rng::genProbabilities(LEAF_BLOCK_REMOVE_CHANCE, leafBlocks.size());
   for(unsigned int i = 0; i < numMutations; ++i)
   {
      mutateRemoveLeafBlock();
   }

   //3. Extend definition with a neuron
   numMutations = Rng::genProbabilities(NEW_NEURON_IN_DEFINITION_CHANCE, mDefs.size());
   for(unsigned int i = 0; i < numMutations; ++i)
   {
      mutateNewNeuron();
   }

   //4. Remove unconnected neuron
   std::vector<GlobalNodeId> disconnectedNeuronIds = getDisconnectedNeuronIds();
   for(auto& n : disconnectedNeuronIds)
   {
      if(Rng::genProbability(REMOVE_UNCONNECTED_NEURON_CHANCE))
      {
         erase(n);
      }
   }

   //5. Add connection
   auto numNeurons = getNumNeurons();
   numMutations = Rng::genProbabilities(CONNECT, numNeurons);
   for(unsigned int i = 0; i < numMutations; ++i)
   {
      mutateNewConnection();
   }

   //6. Remove connection
   numMutations = Rng::genProbabilities(DISCONNECT, numCons);
   for(unsigned int i = 0; i < numMutations; ++i)
   {
      auto numCons = getComplexity();
      if(numCons > 0)
      {
         erase(std::next(beginConnections(), Rng::genChoise(numCons)));   
      }
   }
}

Genom::GlobalNodeId Genom::getRandomSource() const
{
   //Source can be any node
   auto ranVal = Rng::genChoise(mNumInputs + mNumOutputs + getNumNeurons());
   if(ranVal < mNumInputs)
   {
      GlobalNodeId result;
      result.type = GlobalNodeType::Input;
      result.localId = ranVal;
      result.blockId = 0;
      return result;
   }

   ranVal -= mNumInputs;
   if(ranVal < mNumOutputs)
   {
      GlobalNodeId result;
      result.type = GlobalNodeType::Output;
      result.localId = ranVal;
      result.blockId = 0;
      return result;
   }

   ranVal -= mNumOutputs;
   return std::next(beginNodes(), ranVal)->id;
}

Genom::GlobalNodeId Genom::getRandomTarget() const
{
   //Target can be either hidden node or output node
   auto ranVal = Rng::genChoise(mNumOutputs + getNumNeurons());
   if(ranVal < mNumOutputs)
   {
      GlobalNodeId result;
      result.type = GlobalNodeType::Output;
      result.localId = ranVal;
      result.blockId = 0;
      return result;
   }

   ranVal -= mNumOutputs;
   return std::next(beginNodes(), ranVal)->id;
}

bool Genom::GlobalNodeId::operator == (const GlobalNodeId& other) const
{
   return type == other.type && blockId == other.blockId && localId == other.localId;
}

void Genom::erase(const GlobalNodeId id)
{
   auto blockIter = std::find_if(mBlocks.begin(), mBlocks.end(), [id](auto x){return x.blockId == id.blockId;});
   if(blockIter != mBlocks.end()) //Possible to be false if not hidden neuron
   {
      auto defIter = std::find_if(mDefs.begin(), mDefs.end(), [blockIter](auto x){return x.id == blockIter->definitionId;});
      std::erase_if(defIter->neurons, [id](auto x){return x.id == id.localId;});
   }
}

void Genom::erase(ConnectionsIterator iter)
{
   if(iter.isExternal())
   {
      auto blockIter = std::find_if(mBlocks.begin(), mBlocks.end(), [iter](auto x){return x.blockId == iter.mBlockIter->blockId;});
      blockIter->externalConnections.erase(iter.mExternalIter);
   }
   else
   {
      auto defIter = std::find_if(mDefs.begin(), mDefs.end(), [iter](auto x){return x.id == iter.mDefIter->id;});
      defIter->internalConnections.erase(iter.mInternalIter);
   }
}

const Genom::NeuroBlockDefinition& Genom::createDefinition()
{
   auto newId = std::max_element(mDefs.begin(), mDefs.end(), [](auto x, auto y){return x.id < y.id;})->id + 1;
   NeuroBlockDefinition item {newId, {{0, static_cast<ActivationFunctionType>(Rng::genChoise(NUM_ACTIVATION_FUNCTION_TYPES))}}};
   
   mDefs.push_back(item);
   return mDefs.back();
}

std::vector<Genom::NeuroBlock>::iterator Genom::createBlock(const NeuroBlockDefinition& def, const NeuroBlockId& parentId)
{
   auto newId = std::max_element(mBlocks.begin(), mBlocks.end(), [](auto x, auto y){return x.blockId < y.blockId;})->blockId + 1;
   NeuroBlock newBlock {newId, parentId, def.id};

   auto parentPos = std::find_if(mBlocks.begin(), mBlocks.end(), [parentId](auto x){return x.blockId == parentId;});

   return mBlocks.insert(parentPos + 1, newBlock);
}

void Genom::mutateParameters()
{
   const double PERTURBATION_CHANCE = 0.8;

   for(auto& block : mBlocks)
   {
      for(auto& c : block.externalConnections)
      {
         if(Rng::genProbability(PERTURBATION_CHANCE))
         {
            c.weight += Rng::genPerturbation();
         }
         else
         {
            c.weight = Rng::genWeight();
         }
      }
   }

   for(auto& def : mDefs)
   {
      for(auto& c : def.internalConnections)
      {
         if(Rng::genProbability(PERTURBATION_CHANCE))
         {
            c.weight += Rng::genPerturbation();
         }
         else
         {
            c.weight = Rng::genWeight();
         }
      }
   }
}

void Genom::crossoverParametersFrom(const Genom& other)
{
   for(std::size_t i = 0; i < mBlocks.size(); ++i)
   {
      for(std::size_t j = 0; j < mBlocks[i].externalConnections.size(); ++j)
      {
         mBlocks[i].externalConnections[j].weight = 
            (mBlocks[i].externalConnections[j].weight + other.mBlocks[i].externalConnections[j].weight) / 2.0;
      }
   }

   for(std::size_t i = 0; i < mDefs.size(); ++i)
   {
      for(std::size_t j = 0; j < mDefs[i].internalConnections.size(); ++j)
      {
         mDefs[i].internalConnections[j].weight = 
            (mDefs[i].internalConnections[j].weight + other.mDefs[i].internalConnections[j].weight) / 2.0;
      }
   }
}

void Genom::updateWeight(ConnectionsIterator iter, const double newWeight)
{
   if(iter.isExternal())
   {
      iter.mExternalIter->weight = newWeight;
   }
   else
   {
      iter.mInternalIter->weight = newWeight;
   }
}

void Genom::operator= (const Genom& other)
{
   mNumInputs = other.mNumInputs;
   mNumOutputs = other.mNumOutputs;
   mDefs = other.mDefs;
   mBlocks = other.mBlocks;
}

bool Genom::operator==(const Genom& other) const
{
   bool result = false;

   if(mNumOutputs == other.mNumOutputs && mNumInputs == other.mNumInputs && mDefs.size() == other.mDefs.size() && mBlocks.size() == other.mBlocks.size())
   {
      result = mDefs == other.mDefs && mBlocks == other.mBlocks;
   }

   return result;
}

Genom Genom::createMinimal(const std::size_t numInputs, const std::size_t numOutputs)
{
   Genom g(numInputs, numOutputs);

   for(std::size_t i = 0; i < numInputs; ++i)
   {
      GlobalNodeId input {GlobalNodeType::Input, 0, i};
      for(std::size_t j = 0; j < numOutputs; ++j)
      {
         GlobalNodeId output {GlobalNodeType::Output, 0, j};
         g.mBlocks[0].externalConnections.push_back({input, output, Rng::genWeight()});
      }
   }

   return g;
}

std::size_t Genom::getNumNeurons() const
{
   return std::distance(beginNodes(), endNodes());
}

unsigned int Genom::getComplexity() const
{
   return std::distance(beginConnections(), endConnections());
}

unsigned int Genom::getNumInputs() const
{
   return mNumInputs;
}

unsigned int Genom::getNumOutputs() const
{
   return mNumOutputs;
}

neuroevolution::BinaryOutput& operator << (neuroevolution::BinaryOutput& out, const Genom& g)
{
   g.saveState(out);
   return out;
}

neuroevolution::BinaryInput& operator >> (neuroevolution::BinaryInput& in, Genom& g)
{
   Genom::loadState(in, g);
   return in;
}

void Genom::loadState(neuroevolution::BinaryInput& in, Genom& g)
{
   std::size_t ins = 0;
   std::size_t outs = 0;

   in >> ins;
   in >> outs;

   g = Genom(ins, outs);

   in >> g.mDefs;
   in >> g.mBlocks;
}

void Genom::saveState(neuroevolution::BinaryOutput& out) const
{
   out << mNumInputs;
   out << mNumOutputs;
   out << mDefs;
   out << mBlocks;
}

Genom::ConnectionsIterator Genom::beginConnections() const
{
   //Remove const cast in final impl
   return ConnectionsIterator(const_cast<Genom&>(*this), false);
}

Genom::ConnectionsIterator Genom::endConnections() const
{
   //Remove const cast in final impl
   return ConnectionsIterator(const_cast<Genom&>(*this), true);
}

Genom::NodesIterator Genom::beginNodes() const
{
   return NodesIterator(*this, false);
}

Genom::NodesIterator Genom::endNodes() const
{
   return NodesIterator(*this, true);
}

Genom::ConnectionsIterator::ConnectionsIterator(Genom& genom, const bool isEnd)
: mGenom(&genom)
{
   if(!isEnd)
   {
      mBlockIter = mGenom->mBlocks.begin();
      mDefIter = mGenom->mDefs.begin();
      findSuitableBlock();
   }
   else
   {
      mBlockIter = mGenom->mBlocks.end();
      mDefIter = mGenom->mDefs.end();
   }
}

bool Genom::ConnectionsIterator::isExternal() const
{
   return mInternalIter == mDefIter->internalConnections.end();
}

bool Genom::ConnectionsIterator::operator != (const ConnectionsIterator& other) const
{
   return !operator==(other);
}

bool Genom::ConnectionsIterator::operator == (const ConnectionsIterator& other) const
{
   if(mBlockIter == mGenom->mBlocks.end() && other.mBlockIter == mGenom->mBlocks.end()) 
   {
      return true;
   }

   return mBlockIter == other.mBlockIter &&
      mDefIter == other.mDefIter && 
      mInternalIter == other.mInternalIter &&
      mExternalIter == other.mExternalIter;
}

Genom::ConnectionsIterator& Genom::ConnectionsIterator::operator ++()
{
   if(mInternalIter != mDefIter->internalConnections.end())
   {
      mInternalIter++;
   }
   else if(mInternalIter == mDefIter->internalConnections.end())
   {
      mExternalIter++;
   }

   if(mInternalIter == mDefIter->internalConnections.end() && mExternalIter == mBlockIter->externalConnections.end())
   {
      mBlockIter++;
      if(mBlockIter != mGenom->mBlocks.end())
      {
         mDefIter =  std::find_if(mGenom->mDefs.begin(), mGenom->mDefs.end(), [this](auto x){return x.id == mBlockIter->definitionId;});
         findSuitableBlock();
      }  
   }

   return *this;
}

const Genom::ExtConnectionDef* Genom::ConnectionsIterator::operator -> () const
{
   updateValue();
   return &mValue;
}

const Genom::ExtConnectionDef& Genom::ConnectionsIterator::operator *() const
{
   updateValue();
   return mValue;
}

void Genom::ConnectionsIterator::updateValue() const
{
   if(mInternalIter != mDefIter->internalConnections.end())
   {
      mValue = ExtConnectionDef{{GlobalNodeType::Local, mBlockIter->blockId, mInternalIter->srcNodeId}, 
         {GlobalNodeType::Local, mBlockIter->blockId, mInternalIter->dstNodeId}, 
         mInternalIter->weight
         };
   }
   else
   {
      mValue = *mExternalIter;
   }
}

void Genom::ConnectionsIterator::findSuitableBlock()
{
   while(mDefIter->internalConnections.empty() && mBlockIter->externalConnections.empty())
   {
      mBlockIter++;
      if(mBlockIter == mGenom->mBlocks.end())
      {
         return;
      } 

      mDefIter = std::find_if(mGenom->mDefs.begin(), mGenom->mDefs.end(), [this](auto x){return x.id == mBlockIter->definitionId;});
   }

   mInternalIter = mDefIter->internalConnections.begin();
   mExternalIter = mBlockIter->externalConnections.begin();
}

Genom::NodesIterator::NodesIterator(const Genom& genom, const bool isEnd)
: mGenom(&genom)
{
   if(!isEnd)
   {
      mBlockIter = mGenom->mBlocks.begin();
      mDefIter = mGenom->mDefs.begin();
      findSuitableBlock();
   }
   else
   {
      mBlockIter = mGenom->mBlocks.end();
      mDefIter = mGenom->mDefs.end();
   }
}

bool Genom::NodesIterator::operator != (const NodesIterator& other) const
{
   return !operator==(other);
}

bool Genom::NodesIterator::operator == (const NodesIterator& other) const
{
   if(mBlockIter == mGenom->mBlocks.end() && other.mBlockIter == mGenom->mBlocks.end()) 
   {
      return true;
   }

   return mBlockIter == other.mBlockIter &&
      mDefIter == other.mDefIter && 
      mNodeIter == other.mNodeIter;
}

Genom::NodesIterator& Genom::NodesIterator::operator ++()
{
   if(mNodeIter != mDefIter->neurons.end())
   {
      mNodeIter++;
   }

   if(mNodeIter == mDefIter->neurons.end())
   {
      mBlockIter++;
      if(mBlockIter != mGenom->mBlocks.end())
      {
         mDefIter = std::find_if(mGenom->mDefs.begin(), mGenom->mDefs.end(), [this](auto x){return x.id == mBlockIter->definitionId;});
         findSuitableBlock();
      }
   }

   return *this;
}

void Genom::NodesIterator::findSuitableBlock()
{
   while(mDefIter->neurons.empty())
   {
      mBlockIter++;
      if(mBlockIter == mGenom->mBlocks.end())
      {
         return;
      } 

      mDefIter = std::find_if(mGenom->mDefs.begin(), mGenom->mDefs.end(), [this](auto x){return x.id == mBlockIter->definitionId;});
   }

   mNodeIter = mDefIter->neurons.begin();
}

const Genom::GlobalNeuronDef* Genom::NodesIterator::operator -> () const
{
   updateValue();
   return &mValue;
}

const Genom::GlobalNeuronDef& Genom::NodesIterator::operator *() const
{
   updateValue();
   return mValue;
}

void Genom::NodesIterator::updateValue() const
{
   mValue = GlobalNeuronDef{{GlobalNodeType::Local, mBlockIter->blockId, mNodeIter->id}, mNodeIter->acType};
}

Genom::GlobalNodeId::GlobalNodeId(const GlobalNodeType _type, const NeuroBlockId _blockId, const LocalNodeId _localId)
: type(_type)
, blockId(_blockId)
, localId(_localId)
{

}

neuroevolution::BinaryOutput& operator << (neuroevolution::BinaryOutput& out, const Genom::NeuroBlockDefinition& g)
{
   out << g.id;
   out << g.internalConnections;
   out << g.neurons;

   return out;
}

neuroevolution::BinaryInput& operator >> (neuroevolution::BinaryInput& in, Genom::NeuroBlockDefinition& g)
{
   in >> g.id;
   in >> g.internalConnections;
   in >> g.neurons;

   return in;
}

neuroevolution::BinaryOutput& operator << (neuroevolution::BinaryOutput& out, const Genom::NeuroBlock& g)
{
   out << g.blockId;
   out << g.definitionId;
   out << g.parentBlockId;
   out << g.externalConnections;

   return out;
}

neuroevolution::BinaryInput& operator >> (neuroevolution::BinaryInput& in, Genom::NeuroBlock& g)
{
   in >> g.blockId;
   in >> g.definitionId;
   in >> g.parentBlockId;
   in >> g.externalConnections;

   return in;
}

neuroevolution::BinaryOutput& operator << (neuroevolution::BinaryOutput& out, const Genom::IntConnectionDef& g)
{
   out << g.srcNodeId;
   out << g.dstNodeId;
   out << g.weight;

   return out;
}

neuroevolution::BinaryInput& operator >> (neuroevolution::BinaryInput& in, Genom::IntConnectionDef& g)
{
   in >> g.srcNodeId;
   in >> g.dstNodeId;
   in >> g.weight;

   return in;
}

neuroevolution::BinaryOutput& operator << (neuroevolution::BinaryOutput& out, const Genom::ExtConnectionDef& g)
{
   out << g.srcNodeId;
   out << g.dstNodeId;
   out << g.weight;

   return out;
}

neuroevolution::BinaryInput& operator >> (neuroevolution::BinaryInput& in, Genom::ExtConnectionDef& g)
{
   in >> g.srcNodeId;
   in >> g.dstNodeId;
   in >> g.weight;

   return in;
}

neuroevolution::BinaryOutput& operator << (neuroevolution::BinaryOutput& out, const Genom::GlobalNodeId& g)
{
   out << static_cast<int>(g.type);
   out << g.blockId;
   out << g.localId;

   return out;
}

neuroevolution::BinaryInput& operator >> (neuroevolution::BinaryInput& in, Genom::GlobalNodeId& g)
{
   int tmp;
   in >> tmp;
   g.type = static_cast<Genom::GlobalNodeType>(tmp);

   std::size_t v;
   in >> v;
   g.blockId = v;
   
   in >> v;
   g.localId = v;

   return in;
}

neuroevolution::BinaryOutput& operator << (neuroevolution::BinaryOutput& out, const Genom::LocalNeuronDef& g)
{
   out << static_cast<int>(g.acType);
   out << g.id;

   return out;
}

neuroevolution::BinaryInput& operator >> (neuroevolution::BinaryInput& in, Genom::LocalNeuronDef& g)
{
   int tmp;
   in >> tmp;
   g.acType = static_cast<ActivationFunctionType>(tmp);

   in >> g.id;

   return in;
}

}