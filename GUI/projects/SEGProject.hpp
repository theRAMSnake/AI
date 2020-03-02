#pragma once
#include "neuroevolution/IPlayground.hpp"
#include "ProjectBase.hpp"
#include <boost/property_tree/ptree.hpp>
#include "seg/algorithm.hpp"

class SEGPopulation;
class SEGProject : public ProjectBase
{
public:
   SEGProject(const boost::property_tree::ptree& cfg, neuroevolution::IPlayground& pg);

   void step() override;
   void saveState(const std::string& filename) override;
   void loadState(const std::string& filename) override;
   
   const IPopulation& getPopulation() const override;
   Engine getEngine() const override;
   void getRawOut(std::stringstream& out) const override;
   
private:
   void reconfigure() override;

   std::unique_ptr<SEGPopulation> mPops;
   seg::Algorithm mImpl;
};