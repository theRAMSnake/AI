#pragma once

#include <boost/property_tree/ptree.hpp>
#include "neuroevolution/neuro_net.hpp"
#include "IPopulation.hpp"

class IProject
{
public:
   virtual void step() = 0;
   virtual void play(neuroevolution::NeuroNet& ann) = 0;
   virtual void saveState(const std::string& filename) = 0;
   virtual void loadState(const std::string& filename) = 0;
   virtual void updateConfig(const boost::property_tree::ptree& newCfg) = 0;
   virtual void setGeneration(const unsigned int generation) = 0;

   virtual const boost::property_tree::ptree& getConfig() const = 0;
   virtual const unsigned int getGeneration() const = 0;
   virtual std::string getEngine() const = 0;
   virtual const IPopulation& getPopulation() const = 0;
   virtual void getRawOut(std::stringstream& out) const = 0;
   virtual const unsigned int getAutosavePeriod() const = 0;
   virtual neuroevolution::IPlayground& getPlayground() = 0;

   virtual ~IProject(){}
};