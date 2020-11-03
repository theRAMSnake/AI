#pragma once
#include "genom.hpp"
#include <vector>
#include "neuroevolution/IPlayground.hpp"

namespace snakega
{

class Pop
{
public:
   Pop();
   Pop(const Genom& genom);
   
   Pop& operator = (const Pop& other);

   void mutateStructure();
   void mutateParameters();

   Genom mGenom;
   neuroevolution::Fitness fitness;
};

}