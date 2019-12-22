#include "rng.hpp"
#include <boost/random.hpp>

namespace neat
{

boost::random::mt19937 rng;
boost::uniform_real<> doubleGen(-1.0, 1.0);
boost::uniform_real<> perturbationGen(-0.05, 0.05);
boost::uniform_real<> realGen(0, 1.0);

void Rng::seed(const unsigned int seed)
{
   rng.seed(seed);
}

bool Rng::genProbability(const double chance)
{
   return realGen(rng) < chance;
}

double Rng::genWeight()
{
   return doubleGen(rng);
}

double Rng::genPerturbation()
{
   return perturbationGen(rng);
}

unsigned int Rng::genChoise(const unsigned int numOptions)
{
   return rng() % numOptions;
}

unsigned int Rng::gen32()
{
    return rng();
}

}
