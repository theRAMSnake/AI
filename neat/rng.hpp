#pragma once

namespace neat
{

class Rng
{
public:
   static void seed(const unsigned int seed);
   static bool genProbability(const double chance);
   static double genWeight();
   static double genPerturbation();
   static unsigned int genChoise(const unsigned int numOptions);
   static unsigned int gen32();
};

}