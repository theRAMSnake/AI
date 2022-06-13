#pragma once
#include "gacommon/natural_selection.hpp"
#include "snake4/snake4.hpp"
#include "gacommon/IPlayground.hpp"
#include "ProjectBase.hpp"
#include <boost/property_tree/ptree.hpp>

class PopulationAdapter : public IPopulation
{
public:
   PopulationAdapter(const snake4::Algorithm& src, gacommon::IODefinition io);

   double getAverageFitness() const override;
   double getTopFitness() const override;
   double getAverageComplexity() const override;
   std::size_t size() const override;
   unsigned int getNumSpecies() const override;
   std::vector<SpecieResults> getSpecies() const override;
   std::unique_ptr<gacommon::IAgent> createAgent(const PopResult& pop) const override;

private:
   gacommon::IODefinition mIO;
   const snake4::Algorithm& mSrc;
};
class S4Project : public ProjectBase
{
public:
   S4Project(const boost::property_tree::ptree& cfg, gacommon::IPlayground& pg);

   void step() override;
   void saveState(const std::string& filename) override;
   void loadState(const std::string& filename) override;
   Engine getEngine() const override;

   const IPopulation& getPopulation() const override;
   void getRawOut(std::stringstream& out) const override;

private:
   void reconfigure() override;

   gacommon::IODefinition mIO;
   std::unique_ptr<snake4::Algorithm> mAlgo;
   std::unique_ptr<PopulationAdapter> mPops;
};
