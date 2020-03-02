#include "SEGProject.hpp"
#include <seg/agent.hpp>
#include <boost/property_tree/json_parser.hpp>

class SEGPopulation : public IPopulation
{
public:
   using SrcPopsType = decltype(((seg::Algorithm*)nullptr)->getPopulation());

   SEGPopulation(SrcPopsType& src, const unsigned int& memorySize)
   : mSrc(src)
   , mMemSize(memorySize)
   {
      mResults.id = 0;
   }

   virtual double getAverageFitness() const
   {
      return mResults.averageFitness;
   }

   virtual double getAverageComplexity() const
   {
      return mAverageComplexity;
   }

   virtual std::size_t size() const
   {
      return mResults.popResults.size();
   }

   virtual unsigned int getNumSpecies() const
   {
      return mResults.popResults.empty() ? 0 : 1;
   }

   virtual std::vector<SpecieResults> getSpecies() const
   {
      return {mResults};
   }

   virtual std::unique_ptr<neuroevolution::IAgent> createAgent(const PopResult& pop) const
   {
      return std::make_unique<seg::Agent>(mSrc[pop.organismId].graph, mMemSize);
   }

   void update()
   {
      double totalFitness = 0.0;
      double totalComplexity = 0.0;

      mResults.popResults.clear();

      unsigned int popId = 0;
      for(auto& p : mSrc)
      {
         totalFitness += p.fitness;
         totalComplexity += p.graph.size();
         mResults.popResults.push_back({0, popId++, p.fitness, p.graph.size(), p.graph.size()});
      }

      mResults.averageFitness = totalFitness / mSrc.size();
      mAverageComplexity = totalComplexity / mSrc.size();
   }

   double getTopFitness() const override
   {
       return mResults.popResults[0].fitness;
   }

private:
   SrcPopsType& mSrc;
   const unsigned int& mMemSize;
   SpecieResults mResults;
   double mAverageComplexity;
};

//---------------------------------------------------------------------------------------------------

static seg::Config toConfig(const boost::property_tree::ptree& cfg)
{
   seg::Config result;

   result.championsKept = cfg.get<std::size_t>("Selection.Champions Kept");
   result.populationSize = cfg.get<std::size_t>("Basic.Population");
   result.survivalRate = cfg.get<double>("Selection.Survival Rate");
   result.numThreads = cfg.get<unsigned int>("Basic.Threads");
   result.memorySize = cfg.get<unsigned int>("Basic.Memory Size");
   
   return result;
}

SEGProject::SEGProject(const boost::property_tree::ptree& cfg, neuroevolution::IPlayground& pg)
: ProjectBase(cfg, pg)
, mImpl(toConfig(cfg), pg.getDomainGeometry().inputs.size(), pg.getDomainGeometry().outputs.size(), pg.getFitnessEvaluator())
{
   mPops = std::make_unique<SEGPopulation>(mImpl.getPopulation(), cfg.get<unsigned int>("Basic.Memory Size"));
}

void SEGProject::step()
{
   getPlayground().step();
   mImpl.step();
   mPops->update();
   mGeneration++;
}

const IPopulation& SEGProject::getPopulation() const
{
   return *mPops;
}

void SEGProject::saveState(const std::string& filename)
{
   mImpl.saveState(filename);
}

void SEGProject::loadState(const std::string& filename)
{
   mImpl.loadState(filename);
}

void SEGProject::reconfigure()
{
   mImpl.reconfigure(toConfig(getConfig()));
}

Engine SEGProject::getEngine() const
{
   return Engine::Seg;
}

void SEGProject::getRawOut(std::stringstream& out) const
{
   auto& pops = mImpl.getPopulation();

   out << "Generation: " << getGeneration() << std::endl;
   out << "Total population: " << pops.size() << std::endl;
   out << "Average fitness: " << mPops->getAverageFitness() << std::endl;

   for(std::size_t i = 0; i < 10 && i < pops.size(); ++i)
   {
      out << "Pop[" << i << "]: F: " << pops[i].fitness << ", C: " 
         << pops[i].graph.size() << std::endl;
   }
}