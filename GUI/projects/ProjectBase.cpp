#include "ProjectBase.hpp"

const unsigned int ProjectBase::getGeneration() const
{
   return mGeneration;
}

void ProjectBase::setGeneration(const unsigned int generation)
{
   mGeneration = generation;
}

const boost::property_tree::ptree& ProjectBase::getConfig() const
{
   return mConfig;
}

void ProjectBase::updateConfig(const boost::property_tree::ptree& newCfg)
{
   mConfig = newCfg;
   reconfigure();
}

const unsigned int ProjectBase::getAutosavePeriod() const
{
   return mConfig.get<unsigned int>("Basic.Autosave Period");
}

void ProjectBase::play(neuroevolution::IAgent& agent)
{
   getPlayground().play(agent);
}

neuroevolution::IPlayground& ProjectBase::getPlayground()
{
   return mPlayground;
}

ProjectBase::ProjectBase(const boost::property_tree::ptree& cfg, neuroevolution::IPlayground& pg)
: mConfig(cfg)
, mPlayground(pg)
{

}