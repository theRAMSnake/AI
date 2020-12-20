#pragma once
#include "genom.hpp"
#include <vector>
#include "neuroevolution/IPlayground.hpp"
#include "neuroevolution/BinaryIO.hpp"

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

neuroevolution::BinaryOutput& operator << (neuroevolution::BinaryOutput& out, const Pop& p);
neuroevolution::BinaryInput& operator >> (neuroevolution::BinaryInput& in, Pop& p);

const Pop& selectTournament(const std::vector<Pop>& pops);

}