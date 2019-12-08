#include "NeatProject.hpp"

NeatProject::NeatProject(const boost::property_tree::ptree& cfg, IPlayground& pg)
: mNeat(toNeatConfig(cfg, pg.getNumInputs(), pg.getNumOutputs()), pg.getFitnessEvaluator())
, mConfig(cfg)
, mPlayground(pg)
{

}

void NeatProject::step()
{
   mPlayground.step();
   mNeat.step();
   mGeneration++;
}

const neat::Population& NeatProject::getPopulation() const
{
   static neat::Population empty(0, 0, 0);
   return mNeat.hasPopulation() ? mNeat.getPopulation() : empty;
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

const boost::property_tree::ptree& NeatProject::getConfig()
{
   return mConfig;
}

void NeatProject::updateConfig(const boost::property_tree::ptree& newCfg)
{
   mConfig = newCfg;
   mNeat.reconfigure(toNeatConfig(mConfig, 0, 0));
}

const unsigned int NeatProject::getAutosavePeriod() const
{
   return mConfig.get<unsigned int>("Basic.Autosave Period");
}

void NeatProject::play(const neat::Genom& g)
{
   mPlayground.play(g);
}