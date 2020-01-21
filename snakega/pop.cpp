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

}