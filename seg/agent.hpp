#pragma once
#include "graph.hpp"
#include "interpreter.hpp"
#include <neuroevolution/IPlayground.hpp>

namespace seg
{

class Agent : public neuroevolution::IAgent
{
public:
    Agent(const Graph& g, const std::size_t memorySize);

    void reset() override;
    unsigned int run(const double* input) override;
    void toBinaryStream(std::ofstream& stream) const override;

private:
    const Graph& mGraph;
    Memory mMemory;
};

}