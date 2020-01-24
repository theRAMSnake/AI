#include "pop.hpp"

namespace snakega
{

Pop::Pop()
: mGenom(0, 0)
{
   throw -1;
}

Pop::Pop(const Genom& genom)
: mGenom(genom)
, mFitness(0)
{

}

Pop& Pop::operator = (const Pop& other)
{
   mFitness = other.mFitness;
   mGenom = other.mGenom;

   return *this;
}

void Pop::mutateStructure(const MutationConfig& mutationConfig)
{
   mFitness = 0;
   mGenom.mutateStructure(mutationConfig);
}   

void Pop::mutateParameters(const MutationConfig& mutationConfig)
{
   mFitness = 0;
   mGenom.mutateParameters(mutationConfig);
}


}