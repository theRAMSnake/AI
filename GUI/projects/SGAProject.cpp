#include "SGAProject.hpp"
#include <boost/property_tree/json_parser.hpp>
#include "snakega/decoder.hpp"

class SGAPopulation : public IPopulation
{
public:
   using SrcPopsType = decltype(((snakega::Algorithm*)nullptr)->getPopulation());

   SGAPopulation(SrcPopsType& src, neuroevolution::DomainGeometry geometry)
   : mSrc(src)
   , mGeometry(geometry)
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
      return std::make_unique<neuroevolution::NNAgent>(
         mGeometry.inputs.size(),
         mGeometry.outputs.size(),
         snakega::GenomDecoder::decode(mSrc[pop.organismId].mGenom)
         );
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
         totalComplexity += p.mGenom.getComplexity();
         mResults.popResults.push_back({0, popId++, p.fitness, p.mGenom.getComplexity(), p.mGenom.getNumNeurons()});
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
   SpecieResults mResults;
   neuroevolution::DomainGeometry mGeometry;
   double mAverageComplexity;
};

//---------------------------------------------------------------------------------------------------

snakega::Config toConfig(const boost::property_tree::ptree& cfg)
{
   snakega::Config result;

   result.championsKept = cfg.get<std::size_t>("Selection.Champions Kept");
   result.exploitationDepth = cfg.get<std::size_t>("Exploitation.Depth");
   result.exploitationSize = cfg.get<std::size_t>("Exploitation.Size");
   result.populationSize = cfg.get<std::size_t>("Basic.Population");
   result.survivalRate = cfg.get<double>("Selection.Survival Rate");
   result.numThreads = cfg.get<unsigned int>("Basic.Threads");
   
   return result;
}

SGAProject::SGAProject(const boost::property_tree::ptree& cfg, neuroevolution::IPlayground& pg)
: ProjectBase(cfg, pg)
, mImpl(toConfig(cfg), pg.getDomainGeometry(), pg.getFitnessEvaluator())
{
   mPops = std::make_unique<SGAPopulation>(mImpl.getPopulation(), pg.getDomainGeometry());
}

void SGAProject::step()
{
   auto start = std::chrono::high_resolution_clock::now(); 

   getPlayground().step();
   mImpl.step();
   mPops->update();
   mGeneration++;

   auto stop = std::chrono::high_resolution_clock::now(); 

   mLastGenTime = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
}

const IPopulation& SGAProject::getPopulation() const
{
   return *mPops;
}

void SGAProject::saveState(const std::string& filename)
{
   mImpl.saveState(filename);
}

void SGAProject::loadState(const std::string& filename)
{
   mImpl.loadState(filename);
}

void SGAProject::reconfigure()
{
   mImpl.reconfigure(toConfig(getConfig()));
}

Engine SGAProject::getEngine() const
{
   return Engine::SnakeGA;
}

void SGAProject::getRawOut(std::stringstream& out) const
{
   auto& pops = mImpl.getPopulation();

   out << "Generation: " << getGeneration() << std::endl;
   out << "Total population: " << pops.size() << std::endl;
   out << "Average fitness: " << mPops->getAverageFitness() << std::endl;
   out << "Last generation time: " << mLastGenTime.count() << "s" << std::endl;

   for(std::size_t i = 0; i < 10 && i < pops.size(); ++i)
   {
      out << "Pop[" << i << "]: F: " << pops[i].fitness << ", N: " << pops[i].mGenom.getNumNeurons() << ", C: " 
         << pops[i].mGenom.getComplexity() << std::endl;
   }
}