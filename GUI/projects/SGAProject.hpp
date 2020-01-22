#pragma once
#include "neuroevolution/IPlayground.hpp"
#include "ProjectBase.hpp"
#include <boost/property_tree/ptree.hpp>
#include "snakega/snakega.hpp"

class SGAPopulation;
class SGAProject : public ProjectBase
{
public:
   SGAProject(const boost::property_tree::ptree& cfg, neuroevolution::IPlayground& pg);

   void step() override;
   void saveState(const std::string& filename) override;
   void loadState(const std::string& filename) override;
   
   const IPopulation& getPopulation() const override;
   Engine getEngine() const override;
   void getRawOut(std::stringstream& out) const override;
   
private:
   void reconfigure() override;

   std::unique_ptr<SGAPopulation> mPops;
   snakega::Algorithm mImpl;
};