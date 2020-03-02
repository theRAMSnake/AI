#include "pop.hpp"

namespace snakega
{

Pop::Pop()
: mGenom({}, {})
{
   throw -1;
}

Pop::Pop(const Genom& genom)
: mGenom(genom)
, fitness(0)
{

}

Pop& Pop::operator = (const Pop& other)
{
   fitness = other.fitness;
   mGenom = other.mGenom;

   return *this;
}

void Pop::mutateStructure(const MutationConfig& mutationConfig)
{
   fitness = 0;
   mGenom.mutateStructure(mutationConfig);
}   

void Pop::mutateParameters(const MutationConfig& mutationConfig)
{
   fitness = 0;
   mGenom.mutateParameters(mutationConfig);
}


}