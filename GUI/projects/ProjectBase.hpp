#pragma once
#include "neuroevolution/IPlayground.hpp"
#include "../IProject.hpp"
#include <boost/property_tree/ptree.hpp>

class ProjectBase : public IProject
{
public:
   ProjectBase(const boost::property_tree::ptree& cfg, neuroevolution::IPlayground& pg);

   void setGeneration(const unsigned int generation) override;
   void updateConfig(const boost::property_tree::ptree& newCfg) override;
   void play(neuroevolution::IAgent& agent) override;

   const unsigned int getGeneration() const override;
   const boost::property_tree::ptree& getConfig() const override;
   const unsigned int getAutosavePeriod() const override;
   neuroevolution::IPlayground& getPlayground() override;

protected:
   virtual void reconfigure() = 0;
   unsigned int mGeneration = 0;

private:
   boost::property_tree::ptree mConfig;
   neuroevolution::IPlayground& mPlayground;
};