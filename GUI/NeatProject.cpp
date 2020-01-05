#include "NeatProject.hpp"
#include <boost/property_tree/json_parser.hpp>

neat::Config toNeatConfig(const boost::property_tree::ptree& cfg, const unsigned int numInputs, const unsigned int numOutputs)
{
    neat::Config result;

    result.numInputs = numInputs;
    result.numOutputs = numOutputs;
    result.mutationCfg.addConnectionMutationChance = cfg.get<double>("Mutation.Add Connection");
    result.mutationCfg.addNodeMutationChance = cfg.get<double>("Mutation.Add Node");
    result.mutationCfg.removeNodeMutationChance = cfg.get<double>("Mutation.Remove Node");
    result.mutationCfg.perturbationChance = cfg.get<double>("Mutation.Perturbation");
    result.mutationCfg.removeConnectionMutationChance = cfg.get<double>("Mutation.Remove Connection");
    result.mutationCfg.weightsMutationChance = cfg.get<double>("Mutation.Weights");
    result.populationCfg.mC1_C2 = cfg.get<double>("Speciation.C1/C2");
    result.populationCfg.mC3 = cfg.get<double>("Speciation.C3");
    result.populationCfg.mCompatibilityFactor = cfg.get<double>("Speciation.Compatibility Factor");
    result.populationCfg.size = cfg.get<unsigned int>("Basic.Population");
    result.populationCfg.minterspecieCrossoverPercentage = cfg.get<double>("Speciation.Interspecie Crossover");
    result.numThreads = cfg.get<unsigned int>("Basic.Threads");

    if(result.mutationCfg.addNodeMutationChance != 0.05)
    {
       boost::property_tree::write_json("dbg.out", cfg);
    }
    
    return result;
}

NeatProject::NeatProject(const boost::property_tree::ptree& cfg, IPlayground& pg)
: mNeat(toNeatConfig(cfg, pg.getNumInputs(), pg.getNumOutputs()), 
      cfg.get<int>("Es.Phasing") == 0 ? neat::EvolutionStrategyType::Blend : neat::EvolutionStrategyType::Phasing,
      pg.getFitnessEvaluator())
, mConfig(cfg)
, mPlayground(pg)
{

}

void NeatProject::step()
{
   mPlayground.step();
   mNeat.step();
   mGeneration++;
}

const neat::Population& NeatProject::getPopulation() const
{
   static neat::Population empty({});
   return mNeat.hasPopulation() ? mNeat.getPopulation() : empty;
}

const unsigned int NeatProject::getGeneration() const
{
   return mGeneration;
}

void NeatProject::saveState(const std::string& filename)
{
   mNeat.saveState(filename);
}

void NeatProject::loadState(const std::string& filename)
{
   mNeat.loadState(filename);
}

void NeatProject::setGeneration(const unsigned int generation)
{
   mGeneration = generation;
}

const boost::property_tree::ptree& NeatProject::getConfig()
{
   return mConfig;
}

void NeatProject::updateConfig(const boost::property_tree::ptree& newCfg)
{
   mConfig = newCfg;
   mNeat.reconfigure(toNeatConfig(mConfig, 0, 0), 
      newCfg.get<int>("Es.Phasing") == 0 ? neat::EvolutionStrategyType::Blend : neat::EvolutionStrategyType::Phasing);
}

const unsigned int NeatProject::getAutosavePeriod() const
{
   return mConfig.get<unsigned int>("Basic.Autosave Period");
}

void NeatProject::play(const neat::v2::Genom& g)
{
   mPlayground.play(g);
}

std::string NeatProject::getEsInfo() const
{
   return mNeat.getEsInfo();
}

IPlayground& NeatProject::getPlayground()
{
   return mPlayground;
}