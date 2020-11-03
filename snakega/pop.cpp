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

void Pop::mutateStructure()
{
   fitness = 0;
   mGenom.mutateStructure();
}   

void Pop::mutateParameters()
{
   fitness = 0;
   mGenom.mutateParameters();
}


}