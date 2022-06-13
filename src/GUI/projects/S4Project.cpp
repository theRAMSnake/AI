#include "S4Project.hpp"
#include "gacommon/natural_selection.hpp"
#include "snake4/pop.hpp"
#include "snake4/snake4.hpp"
#include <numeric>

double PopulationAdapter::getAverageFitness() const
{
    return std::accumulate(mSrc.getPopulation().begin(), mSrc.getPopulation().end(), 0.0, [](auto x, auto y){return x + y.fitness;}) / mSrc.getPopulation().size();
}

double PopulationAdapter::getAverageComplexity() const
{
    return std::accumulate(mSrc.getPopulation().begin(), mSrc.getPopulation().end(), 0.0, [](auto x, auto y){return x + y.blocks.size();}) / mSrc.getPopulation().size();
}

std::size_t PopulationAdapter::size() const
{
    return mSrc.getPopulation().size();
}

unsigned int PopulationAdapter::getNumSpecies() const
{
    return 1;
}

std::vector<SpecieResults> PopulationAdapter::getSpecies() const
{
    std::vector<SpecieResults> results;
    results.push_back({0, 0, mSrc.getPopulation()[0].fitness});

    for(auto& s : mSrc.getPopulation())
    {
        unsigned int i = 0;
        results.back().popResults.push_back(PopResult{0, i++, s.fitness, s.blocks.size(), s.blocks.size()});
    }

    return results;
}

std::unique_ptr<gacommon::IAgent> PopulationAdapter::createAgent(const PopResult& pop) const
{
    return mSrc.getPopulation()[pop.organismId].createAgent(mIO);
}

PopulationAdapter::PopulationAdapter(const snake4::Algorithm& src, gacommon::IODefinition io)
: mIO(io)
, mSrc(src)
{

}

double PopulationAdapter::getTopFitness() const
{
    return mSrc.getPopulation()[0].fitness;
}

gacommon::Config toConfig(const boost::property_tree::ptree& cfg)
{
    gacommon::Config result;

    result.populationSize = cfg.get<unsigned int>("Basic.Population");
    result.championsKept = cfg.get<unsigned int>("Selection.Champions Kept");
    result.survivalRate = cfg.get<double>("Selection.Survival Rate");
    result.numThreads = cfg.get<unsigned int>("Basic.Threads");

    return result;
}

S4Project::S4Project(const boost::property_tree::ptree& cfg, gacommon::IPlayground& pg)
: ProjectBase(cfg, pg)
, mIO{pg.getInputs(), pg.getOutputs()}
{
    mAlgo.reset(new snake4::Algorithm(toConfig(cfg), mIO, pg.getFitnessEvaluator()));
    mPops.reset(new PopulationAdapter(*mAlgo, mIO));
}

void S4Project::step()
{
   getPlayground().step();
   mAlgo->step();
   mGeneration++;
}

const IPopulation& S4Project::getPopulation() const
{
   return *mPops;
}

void S4Project::saveState(const std::string& filename)
{
   mAlgo->saveState(filename);
}

void S4Project::loadState(const std::string& filename)
{
   mAlgo->loadState(filename);
}

void S4Project::reconfigure()
{
   mAlgo->reconfigure(toConfig(getConfig()));
}

Engine S4Project::getEngine() const
{
   return Engine::SnakeGA;
}

void S4Project::getRawOut(std::stringstream& out) const
{
   auto& pops = mAlgo->getPopulation();

   out << "Generation: " << getGeneration() << std::endl;
   out << "Total population: " << pops.size() << std::endl;
   out << "Average fitness: " << mPops->getAverageFitness() << std::endl;
   out << "Top 10: " << std::endl;

   int i = 0;
   for(const auto& p : mAlgo->getPopulation())
   {
       out << "F: " << p.fitness << " C: " << p.blocks.size() << std::endl;
       if(i == 10)
       {
           break;
       }
       i++;
   }
}
