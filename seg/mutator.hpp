#pragma once
#include "graph.hpp"
#include "functions.hpp"

namespace seg
{

const double RESULT_NODE_CHANCE = 0.5;
const double NEW_NODE_VS_LINK_CHANCE = 0.75;
const double VAL_VS_FUNC_CHANCE = 0.5;
const double PRIMITIVE_VS_EXTENSION_CHANCE = 0.8;

struct MutationConfig
{
   double insertChance = 0.33;
   double removeChance = 0.33;
   double modifyChance = 0.33;
};

class Mutator
{
public:
   Mutator(const MutationConfig& cfg, Graph& g, const std::size_t memSize, const std::size_t numInputs, const unsigned int numCommands);

   void mutate();

private:
   void mutateInsertion();
   void mutateRemoval();
   void mutateModification();

   void insertActionOrLinkNode(const NodeId parent);
   Expression genExpression();
   VAL genValue();

   const MutationConfig mCfg; 
   Graph& mGraph;
   const std::size_t mMemSize; 
   const std::size_t mNumInputs;
   const unsigned int mNumCommands;
   FunctionLibrary mPrimitives;
   FunctionLibrary mExtensions;
};

}