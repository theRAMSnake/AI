#pragma once
#include "genom.hpp"
#include <vector>
#include <cstddef>

namespace neat
{

class NeuroNet
{
public:
   NeuroNet(const Genom& genotype);

   std::vector<double> activateOneShot(const std::vector<double>& input);

private:
   const Genom& mGenotype;
};

}