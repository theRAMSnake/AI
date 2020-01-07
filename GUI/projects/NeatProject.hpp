#pragma once
#include "neat/neat.hpp"
#include "neuroevolution/IPlayground.hpp"
#include "../IProject.hpp"
#include <boost/property_tree/ptree.hpp>

class NeatProject : public IProject
{
public:
   NeatProject(const boost::property_tree::ptree& cfg, IPlayground& pg);

   void step();
   void saveState(const std::string& filename);
   void loadState(const std::string& filename);
   void setGeneration(const unsigned int generation);
   void play(neuroevolution::NeuroNet& ann) override;

   const IPopulation& getPopulation() const override;
   const unsigned int getGeneration() const override;
   const boost::property_tree::ptree& getConfig() const override;
   std::string getEngine() const override;
   const unsigned int getAutosavePeriod() const;
   std::string getEsInfo() const;
   IPlayground& getPlayground();

   void updateConfig(const boost::property_tree::ptree& newCfg);

private:
   unsigned int mGeneration = 0;
   neat::Neat mNeat;
   boost::property_tree::ptree mConfig;
   IPlayground& mPlayground;
   NeatPopulation mPops;
};