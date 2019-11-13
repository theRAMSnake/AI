#include "NeatController.hpp"

NeatController::NeatController(IPlayground& pg)
: mNeat(pg.getConfig(), pg.getFitnessEvaluator())
{

}

void NeatController::step()
{
   mNeat.step();
   mGeneration++;
}

const neat::Population& NeatController::getPopulation() const
{
   return mNeat.getPopulation();
}

const unsigned int NeatController::getGeneration() const
{
   return mGeneration;
}

void NeatController::saveState(const std::string& filename)
{
    mNeat.saveState(filename);
}

void NeatController::loadState(const std::string& filename)
{
    mNeat.loadState(filename);
}