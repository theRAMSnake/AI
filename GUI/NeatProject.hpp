#pragma once
#include "neat/neat.hpp"
#include "IPlayground.hpp"
#include <boost/property_tree/ptree.hpp>

class NeatProject
{
public:
   NeatProject(IPlayground& pg);

   void step();
   void saveState(const std::string& filename);
   void loadState(const std::string& filename);
   void setGeneration(const unsigned int generation);

   const neat::Population& getPopulation() const;
   const unsigned int getGeneration() const;
   const boost::property_tree::ptree& getConfig();

private:
   unsigned int mGeneration = 0;
   boost::property_tree::ptree mConfig;
   neat::Neat mNeat;
   IPlayground& mPlayground;
};