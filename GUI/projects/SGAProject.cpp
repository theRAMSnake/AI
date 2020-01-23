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
      return 0;
   }

   virtual std::vector<SpecieResults> getSpecies() const
   {
      return {mResults};
   }

   virtual std::unique_ptr<neuroevolution::NeuroNet> createAnn(const PopResult& pop) const
   {
      return snakega::GenomDecoder::decode(mGeometry, mSrc[pop.organismId].mGenom);
   }

   void update()
   {
      double totalFitness = 0.0;
      double totalComplexity = 0.0;

      for(auto& p : mSrc)
      {
         totalFitness += p.mFitness;
         totalComplexity += p.mGenom.getComplexity();
      }

      mResults.averageFitness = totalFitness / mSrc.size();
      mAverageComplexity = totalComplexity / mSrc.size();
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
   getPlayground().step();
   mImpl.step();
   mPops->update();
   mGeneration++;
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

   for(std::size_t i = 0; i < 10 && i < pops.size(); ++i)
   {
      out << "Pop[" << i << "]: F: " << pops[i].mFitness << ", N: " << pops[i].mGenom.getNumNeurons() << ", C: " 
         << pops[i].mGenom.getComplexity() << std::endl;
   }
}