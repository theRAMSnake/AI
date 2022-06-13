#pragma once
#include "neat/neat.hpp"
#include "gacommon/IPlayground.hpp"
#include "ProjectBase.hpp"
#include <boost/property_tree/ptree.hpp>
#include "NeatPopulation.hpp"

class NeatProject : public ProjectBase
{
public:
   NeatProject(const boost::property_tree::ptree& cfg, gacommon::IPlayground& pg);

   void step() override;
   void saveState(const std::string& filename) override;
   void loadState(const std::string& filename) override;
   Engine getEngine() const override;
   
   const IPopulation& getPopulation() const override;
   void getRawOut(std::stringstream& out) const override;
   
private:
   void reconfigure() override;

   std::unique_ptr<neat::Neat> mNeat;
   std::unique_ptr<NeatPopulation> mPops;
};
