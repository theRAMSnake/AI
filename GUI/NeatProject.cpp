#include "NeatProject.hpp"

NeatProject::NeatProject(IPlayground& pg)
: mNeat(pg.getConfig(), pg.getFitnessEvaluator())
{

}

void NeatProject::step()
{
   mNeat.step();
   mGeneration++;
}

const neat::Population& NeatProject::getPopulation() const
{
   return mNeat.getPopulation();
}

const unsigned int NeatProject::getGeneration() const
{
   return mGeneration;
}

void NeatProject::saveState(const std::string& filename)
{
   mNeat.saveState(filename);
}

void NeatProject::loadState(const std::string& filename)
{
   mNeat.loadState(filename);
}

void NeatProject::setGeneration(const unsigned int generation)
{
   mGeneration = generation;
}