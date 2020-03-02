#include "mutator.hpp"
#include <neuroevolution/rng.hpp>
#include <iostream>

namespace seg
{

//#define SHOUT

//#ifdef SHOUT
//#define SH(x) std::cout << x; std::cout.flush();
//#elif
#define SH(x)
//#endif

bool isSaturated(const Choise& ch)
{
   return std::holds_alternative<If>(ch.selector) || std::holds_alternative<Compare>(ch.selector);
}

Mutator::Mutator(const MutationConfig& cfg, Graph& g, const std::size_t memSize, const std::size_t numInputs, const unsigned int numCommands)
: mCfg(cfg)
, mGraph(g)
, mMemSize(memSize)
, mNumInputs(numInputs)
, mNumCommands(numCommands)
{
   mPrimitives = createPrimitivesLibrary();
   mExtensions = createExtensionLibrary();
}

void Mutator::insertActionOrLinkNode(const NodeId parentId)
{
   NodeId desidedNodeId;
   if(Rng::genProbability(NEW_NODE_VS_LINK_CHANCE))
   {
      NodeId newNodeId;

      if(Rng::genProbability(RESULT_NODE_CHANCE))
      {
         newNodeId = mGraph.addNode(Action{Result{Rng::genChoise(mNumCommands)}});
      }
      else
      {
         newNodeId = mGraph.addNode(Action{Operation{Rng::genChoise(mMemSize), genExpression()}});
         insertActionOrLinkNode(newNodeId);
      }

      desidedNodeId = newNodeId;
   }
   else
   {
      do
      {
         desidedNodeId = mGraph.randomNode().id;
      }
      while(desidedNodeId == parentId);
   }

   auto& parent = mGraph.get(parentId);
   if(std::holds_alternative<Choise>(parent.payload))
   {
      auto& ch = std::get<Choise>(parent.payload);
      ch.options.push_back(desidedNodeId);

      if(std::holds_alternative<Switch>(ch.selector))
      {
         std::get<Switch>(ch.selector).anchors.push_back(Rng::genWeight());
      }
      else if(std::holds_alternative<RandomWeighted>(ch.selector))
      {
         std::get<RandomWeighted>(ch.selector).weights.push_back(Rng::genWeight());
      }
   }
   else if(std::holds_alternative<Action>(parent.payload))
   {
      std::get<Operation>(std::get<Action>(parent.payload)).next = desidedNodeId;
   }
}

void Mutator::mutateInsertion()
{
   auto currNodeId = mGraph.randomNode().id;

   //1. X is non saturated choise - expand with new option
   if(std::holds_alternative<Choise>(mGraph.get(currNodeId).payload) && !isSaturated(std::get<Choise>(mGraph.get(currNodeId).payload)))
   {
      SH(".1");
      insertActionOrLinkNode(currNodeId);
   }

   //2. X is saturated choise/operation/result - change to choise with X as a child
   else
   {
      SH(".2");
      auto oldPayload = mGraph.get(currNodeId).payload;
      auto nodeId = mGraph.addNode(oldPayload); //Old node is kept as copy, now we only need to reference it

      Choise newChoise;

      newChoise.options.push_back(nodeId);

      int genNewNodesAmount = 1;

      switch(Rng::genChoise(5))
      {
         case 0:
            newChoise.selector = RandomEven{};
            break;

         case 1:
            newChoise.selector = If{genExpression(), genExpression(), static_cast<IfSign>(Rng::genChoise(4))};
            break;

         case 2:
            newChoise.selector = Compare{genExpression(), genExpression()};
            genNewNodesAmount++;
            break;

         case 3:
            newChoise.selector = Switch{genExpression(), {Rng::genWeight(), Rng::genWeight()}};
            break;

         case 4:
            newChoise.selector = RandomWeighted{{Rng::genWeight(), Rng::genWeight()}};
            break;
      }

      mGraph.get(currNodeId).payload = newChoise;
      for(int i = 0; i < genNewNodesAmount; ++i)
      {
         insertActionOrLinkNode(currNodeId);
      }
   }

   SH(" id:" + std::to_string(currNodeId));
}

void Mutator::mutateRemoval()
{
   auto currNodeId = mGraph.randomNode().id;
   auto& payload = mGraph.get(currNodeId).payload;

   if(std::holds_alternative<Choise>(payload))
   {
      auto& ch = std::get<Choise>(payload);

      auto& options = ch.options;
      auto pos = Rng::genChoise(options.size());
      auto id = *(options.begin() + pos);

      //1. X is shrinkable choise - remove one option
      if((std::holds_alternative<RandomEven>(ch.selector) ||
         std::holds_alternative<Switch>(ch.selector) ||
         std::holds_alternative<RandomWeighted>(ch.selector)) &&
         ch.options.size() > 1)
      {
         SH(".1: from " + std::to_string(currNodeId) + " option " + std::to_string(id));
         options.erase((options.begin() + pos));

         //clean Switch/RandomWeight element
         if(std::holds_alternative<Switch>(ch.selector))
         {
            std::get<Switch>(ch.selector).anchors.erase(std::get<Switch>(ch.selector).anchors.begin() + pos);
         }
         else if(std::holds_alternative<RandomWeighted>(ch.selector))
         {
            std::get<RandomWeighted>(ch.selector).weights.erase(std::get<RandomWeighted>(ch.selector).weights.begin() + pos);
         }

         mGraph.removeIfUnreferenced(id);
         return;  
      }
      //2. X is non shrinkable choise - replace with one of the subtrees
      else
      {
         SH(".2 id " + std::to_string(currNodeId));

         if(id != currNodeId)
         {
            auto copiedOptions = options;
            options.clear();

            mGraph.get(currNodeId).payload = mGraph.get(id).payload;

            for(auto o : copiedOptions)
            {
               mGraph.removeIfUnreferenced(o);
            }

            mGraph.fixReferences(id, currNodeId);
         }

         return;
      }
   }  
   else
   {
      auto& action = std::get<Action>(payload);
      //3. X is operation - remove it, by replacing with next
      if(std::holds_alternative<Operation>(action))
      {
         SH(".3");
         auto& op = std::get<Operation>(action);
         mGraph.fixReferences(currNodeId, op.next);
         mGraph.removeIfUnreferenced(currNodeId);
         return;
      }
   }

   //4. other - cannot be removed
   SH(".4 id:" + std::to_string(currNodeId));
}

void Mutator::mutateModification()
{
   auto currNodeId = mGraph.randomNode().id;
   auto& payload = mGraph.get(currNodeId).payload;

   if(std::holds_alternative<Choise>(payload))
   {
      auto& ch = std::get<Choise>(payload);
      if(std::holds_alternative<If>(ch.selector))
      {
         auto& if_ch = std::get<If>(ch.selector);
         if_ch = If{genExpression(), genExpression(), static_cast<IfSign>(Rng::genChoise(4))};
      }
      else if(std::holds_alternative<Compare>(ch.selector))
      {
         auto& cmpch = std::get<Compare>(ch.selector);
         cmpch = Compare{genExpression(), genExpression()};
      }
      else if(std::holds_alternative<Switch>(ch.selector))
      {
         auto& sw = std::get<Switch>(ch.selector);
         sw.A = genExpression();
         for(auto& w : sw.anchors)
         {
            w = Rng::genWeight();
         }
      }   
      else if(std::holds_alternative<RandomWeighted>(ch.selector))
      {
         auto& rw = std::get<RandomWeighted>(ch.selector);
         for(auto& w : rw.weights)
         {
            w = Rng::genWeight();
         }
      }
   }
   else
   {
      auto& action = std::get<Action>(payload);
      if(std::holds_alternative<Operation>(action))
      {
         std::get<Operation>(action).expr = genExpression();
      }
      else
      {
         action = Result{Rng::genChoise(mNumCommands)};
      }
   }
}

void Mutator::mutate()
{
   if(Rng::genProbability(mCfg.insertChance))
   {
      SH("\nINSERT");
      mutateInsertion();
   }
   if(Rng::genProbability(mCfg.modifyChance))
   {
      SH("\nMODIFY");
      mutateModification();
   }
   if(Rng::genProbability(mCfg.removeChance))
   {
      SH("\nREMOVE");
      mutateRemoval();
   }
}

Expression Mutator::genExpression()
{
   if(Rng::genProbability(VAL_VS_FUNC_CHANCE))
   {
      return genValue();
   }
   else
   {
      FUNC result;
      unsigned int numParams;

      if(Rng::genProbability(PRIMITIVE_VS_EXTENSION_CHANCE))
      {
         result.category = FuncCategory::Primitive;
         result.id = Rng::genChoise(mPrimitives.getNumFunctions());
         numParams = mPrimitives.get(result.id).numParams;
      }
      else
      {
         result.category = FuncCategory::Extension;
         result.id = Rng::genChoise(mExtensions.getNumFunctions());
         numParams = mExtensions.get(result.id).numParams;
      }

      for(unsigned int i = 0; i < numParams; ++i)
      {
         result.params[i] = genValue();
      }

      return result;
   }
}

VAL Mutator::genValue()
{
   VAL result;

   switch(Rng::genChoise(3))
   {
      case 0:
         result = Rng::genWeight();
         break;

      case 1:
         result = MemAddress{Rng::genChoise(mMemSize)};
         break;

      case 2:
         result = InputAddress{Rng::genChoise(mNumInputs)};
         break;
   }

   return result;
}

}