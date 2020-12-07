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

void Genom::mutateStructure()
{
   /*const double CONNECTION_TO_BLOCK_CHANCE = 0.005;
   const double NEW_DEFINITION_CHANCE = 0.2;
   const double LEAF_BLOCK_REMOVE_CHANCE = 0.02;
   const double NEW_NEURON_IN_DEFINITION_CHANCE = 0.05;
   const double REMOVE_UNCONNECTED_NEURON_CHANCE = 0.25;
   const double DISCONNECT = 0.005;
   const double CONNECT = 0.005;
   
   //1. Turn a connection to a block (Either new def or copy def)
   auto numCons = getComplexity();
   auto numMutations = Rng::genProbabilities(numCons, CONNECTION_TO_BLOCK_CHANCE);
   for(unsigned int i = 0; i < numMutations; ++i)
   {
      auto conn = std::next(beginConnections(), Rng::genChoise(numCons));
      auto from = conn->srcNodeId;
      auto to = conn->dstNodeId;
      auto parentId = conn.mBlockIter->blockId;

      erase(conn);
      numCons--;

      if(conn.isExternal())
      {
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
            auto& def = *std::next(mDefs.begin(), Rng::genChoise(mDefs.size()));
            auto pos = createBlock(def, parentId);

            auto nodeId = GlobalNodeId{
               GlobalNodeType::Local, 
               pos->blockId, 
               std::next(def.neurons.begin(), Rng::genChoise(def.neurons.size()))->id
            };
            pos->externalConnections.push_back({from, nodeId, Rng::genWeight()});
            pos->externalConnections.push_back({nodeId, to, Rng::genWeight()});
         }
      }
   }
   
   //2. Remove leaf block (and clean up definition possibly)
   std::vector<NeuroBlockId> leafBlocks;
   for(std::size_t i = 1; i < mBlocks.size() - 1; ++i)
   {
      if(mBlocks[i+1].parentBlockId != mBlocks[i].blockId)
      {
         leafBlocks.push_back(mBlocks[i].blockId);
      }
   }
   
   for(auto& lId : leafBlocks)
   {
      if(Rng::genProbability(LEAF_BLOCK_REMOVE_CHANCE))
      {
         auto blockIter = std::find_if(mBlocks.begin(), mBlocks.end(), [lId](auto x){return x.blockId == lId;});
         auto defId = blockIter->definitionId;

         mBlocks.erase(blockIter);

         //If there is not more blocks with this definition - remove the definition
         auto anotherBlockWithSameDefIdIter = std::find_if(mBlocks.begin(), mBlocks.end(), [defId](auto x){return x.definitionId == defId;});
         if(anotherBlockWithSameDefIdIter == mBlocks.end())
         {
            std::erase_if(mDefs, [defId](auto x){return x.id == defId;});
         }
      }
   }

   //3. Extend definition with a neuron
   for(auto& d : mDefs)
   {
      if(Rng::genProbability(NEW_NEURON_IN_DEFINITION_CHANCE))
      {
         auto n1 = std::next(d.neurons.begin(), Rng::genChoise(d.neurons.size()));
         auto n2 = std::next(d.neurons.begin(), Rng::genChoise(d.neurons.size()));

         auto newId = std::max_element(d.neurons.begin(), d.neurons.end(), [](auto x, auto y){return x.id < y.id;})->id + 1;
         d.neurons.push_back({newId, static_cast<ActivationFunctionType>(Rng::genChoise(NUM_ACTIVATION_FUNCTION_TYPES))});
         
         d.internalConnections.push_back({n1->id, newId, Rng::genWeight()});
         d.internalConnections.push_back({newId, n2->id, Rng::genWeight()});
      }
   }

   //4. Remove unconnected neuron
   std::vector<GlobalNodeId> disconnectedNeuronIds;
   for(auto iter = beginNodes(); iter != endNodes(); ++iter)
   {
      auto id = iter->id;
      if(endConnections() == std::find_if(beginConnections(), endConnections(), [id](auto x){return x.srcNodeId == id || x.dstNodeId == id;}))
      {
         disconnectedNeuronIds.push_back(id);
      }
   }

   for(auto& n : disconnectedNeuronIds)
   {
      if(Rng::genProbability(REMOVE_UNCONNECTED_NEURON_CHANCE))
      {
         erase(n);
      }
   }

   //5. Add connection
   auto numNeurons = getNumNeurons();
   numMutations = Rng::genProbabilities(numNeurons, CONNECT);
   for(unsigned int i = 0; i < numMutations; ++i)
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

   //6. Remove connection
   numMutations = Rng::genProbabilities(numCons, DISCONNECT);
   for(unsigned int i = 0; i < numMutations; ++i)
   {
      erase(std::next(beginConnections(), Rng::genChoise(numCons)));
      numCons--;
   }*/
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

   return mBlocks.insert(parentPos, newBlock);
}

void Genom::mutateParameters()
{
   /*const double PERTURBATION_CHANCE = 0.8;

   for(auto iter = beginConnections(); iter != endConnections(); ++iter)
   {
      if(Rng::genProbability(PERTURBATION_CHANCE))
      {
         updateWeight(iter, iter->weight + Rng::genPerturbation());
      }
      else
      {
         updateWeight(iter, Rng::genWeight());
      }
   }*/
}

void Genom::crossoverParametersFrom(const Genom& other)
{
   /*auto i1 = beginConnections();
   auto e1 = endConnections();
   auto i2 = other.beginConnections();

   while(i1 != e1)
   {
      updateWeight(i1, (i1->weight + i2->weight) / 2.0);

      ++i1;
      ++i2;
   }*/
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
   if(other.mNumInputs != mNumInputs || other.mNumOutputs != mNumOutputs)
   {
      throw -1;
   }

   mDefs = other.mDefs;
   mBlocks = other.mBlocks;
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

Genom Genom::loadState(std::ifstream& s, const std::size_t numInputs, const std::size_t numOutputs)
{
   /*Genom g(numInputs, numOutputs);

   loadBlock(g.mRoot, s);

   return g;*/
   throw -1;
}

void Genom::loadBlock(NeuroBlock& block, std::ifstream& s)
{
   /*s.read((char*)&block.blockId, sizeof(NeuroBlockId));
   s.read((char*)&block.definitionId, sizeof(NeuroBlockDefinitionId));

   std::size_t sz = 0;
   s.read((char*)&sz, sizeof(std::size_t));
   block.externalConnections.resize(sz);
   s.read((char*)block.externalConnections.data(), sizeof(ConnectionDef) * sz);

   s.read((char*)&sz, sizeof(std::size_t));
   block.subBlockDefinitions.resize(sz);
   for(std::size_t k = 0; k < sz; ++k)
   {
      auto p = &block.subBlockDefinitions[k];
      s.read((char*)p, sizeof(NeuroBlockDefinitionId));

      std::size_t ssz = 0;
      s.read((char*)&ssz, sizeof(std::size_t));
      p->internalConnections.resize(sz);
      s.read((char*) p->internalConnections.data(), sizeof(ConnectionDef) * ssz);

      s.read((char*)&ssz, sizeof(std::size_t));
      p->neurons.resize(sz);
      s.read((char*) p->neurons.data(), sizeof(NeuronDef) * ssz);
   }

   s.read((char*)&sz, sizeof(std::size_t));
   block.subBlocks.resize(sz);

   for(std::size_t k = 0; k < sz; ++k)
   {
      loadBlock(block.subBlocks[k], s);
   }*/
}

void Genom::saveState(std::ofstream& s) const
{
   //writeBlock(mRoot, s);
}

void Genom::writeBlock(const NeuroBlock& block, std::ofstream& s) const
{
   /*s.write((char*)&block.blockId, sizeof(NeuroBlockId));
   s.write((char*)&block.definitionId, sizeof(NeuroBlockDefinitionId));

   auto sz = block.externalConnections.size();
   s.write((char*)&sz, sizeof(std::size_t));
   s.write((char*)block.externalConnections.data(), sizeof(ConnectionDef) * block.externalConnections.size());

   sz = block.subBlockDefinitions.size();
   s.write((char*)&sz, sizeof(std::size_t));

   for(auto& d : block.subBlockDefinitions)
   {
      s.write((char*)&d.id, sizeof(NeuroBlockDefinitionId));

      sz = d.internalConnections.size();
      s.write((char*)&sz, sizeof(std::size_t));
      s.write((char*)d.internalConnections.data(), sizeof(ConnectionDef) * d.internalConnections.size());

      sz = d.neurons.size();
      s.write((char*)&sz, sizeof(std::size_t));
      s.write((char*)d.neurons.data(), sizeof(NeuronDef) * d.neurons.size());
   }

   sz = block.subBlocks.size();
   s.write((char*)&sz, sizeof(std::size_t));
   for(auto& b : block.subBlocks)
   {
      writeBlock(b, s);
   }*/
}

Genom::ConnectionsIterator Genom::beginConnections() const
{
   //Remove in final impl
   return ConnectionsIterator(const_cast<Genom&>(*this), false);
}

Genom::ConnectionsIterator Genom::endConnections() const
{
   //Remove in final impl
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
      mDefIter = std::find_if(mGenom->mDefs.begin(), mGenom->mDefs.end(), [this](auto x){return x.id == mBlockIter->definitionId;});

      findSuitableBlock();
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
      mDefIter = std::find_if(mGenom->mDefs.begin(), mGenom->mDefs.end(), [this](auto x){return x.id == mBlockIter->definitionId;});

      findSuitableBlock();
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

}