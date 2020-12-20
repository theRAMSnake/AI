#include "pop.hpp"
#include "neuroevolution/rng.hpp"
#include <iostream>

namespace snakega
{

Pop::Pop()
: mGenom({}, {})
{
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

const Pop& selectTournament(const std::vector<Pop>& pops)
{
    const int NUM_PARTICIPANTS = 8;
    const double PARTICIPANT_CHANCE = 0.25;

    std::vector<std::pair<std::size_t, neuroevolution::Fitness>> indices;
    for(int i = 0; i < NUM_PARTICIPANTS; ++i)
    {
        auto pos = Rng::genChoise(pops.size());
        indices.push_back({pos, pops[pos].fitness});
    }

    std::sort(indices.begin(), indices.end(), [](auto x, auto y){return x.second > y.second;});

    for(int i = 0; i < NUM_PARTICIPANTS - 1; ++i)
    {
        if(Rng::genProbability(PARTICIPANT_CHANCE))
        {
            return pops[indices[i].first];
        }
    }

    return pops[indices[NUM_PARTICIPANTS - 1].first];
}

neuroevolution::BinaryOutput& operator << (neuroevolution::BinaryOutput& out, const Pop& p)
{
    out << p.fitness;
    out << p.mGenom;

    return out;
}

neuroevolution::BinaryInput& operator >> (neuroevolution::BinaryInput& in, Pop& p)
{
    in >> p.fitness;
    in >> p.mGenom;

    return in;
}

}