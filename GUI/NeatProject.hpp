#pragma once
#include "neat/neat.hpp"
#include "IPlayground.hpp"
#include <boost/property_tree/ptree.hpp>

class NeatProject
{
public:
   NeatProject(const boost::property_tree::ptree& cfg, IPlayground& pg);

   void step();
   void saveState(const std::string& filename);
   void loadState(const std::string& filename);
   void setGeneration(const unsigned int generation);
   void play(const neat::Genom& g);

   const neat::Population& getPopulation() const;
   const unsigned int getGeneration() const;
   const boost::property_tree::ptree& getConfig();
   const unsigned int getAutosavePeriod() const;

   void updateConfig(const boost::property_tree::ptree& newCfg);

private:
   unsigned int mGeneration = 0;
   neat::Neat mNeat;
   boost::property_tree::ptree mConfig;
   IPlayground& mPlayground;
};