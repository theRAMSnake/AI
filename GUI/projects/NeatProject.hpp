#pragma once
#include "neat/neat.hpp"
#include "neuroevolution/IPlayground.hpp"
#include "../IProject.hpp"
#include <boost/property_tree/ptree.hpp>
#include "NeatPopulation.hpp"

class NeatProject : public IProject
{
public:
   NeatProject(const boost::property_tree::ptree& cfg, neuroevolution::IPlayground& pg);

   void step() override;
   void saveState(const std::string& filename) override;
   void loadState(const std::string& filename) override;
   void setGeneration(const unsigned int generation) override;
   void play(neuroevolution::NeuroNet& ann) override;
   void updateConfig(const boost::property_tree::ptree& newCfg) override;

   const IPopulation& getPopulation() const override;
   const unsigned int getGeneration() const override;
   const boost::property_tree::ptree& getConfig() const override;
   std::string getEngine() const override;
   void getRawOut(std::stringstream& out) const override;
   const unsigned int getAutosavePeriod() const override;
   neuroevolution::IPlayground& getPlayground() override;

private:
   unsigned int mGeneration = 0;
   neat::Neat mNeat;
   boost::property_tree::ptree mConfig;
   neuroevolution::IPlayground& mPlayground;
   NeatPopulation mPops;
};