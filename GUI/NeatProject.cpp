#include "NeatProject.hpp"

neat::Config toNeatConfig(const boost::property_tree::ptree& cfg, const unsigned int numInputs, const unsigned int numOutputs)
{
   neat::Config result;

   result.numInputs = numInputs;
   result.numOutputs = numOutputs;
   result.addConnectionMutationChance = cfg.get<double>("Mutation.Add Connection");
   result.addNodeMutationChance = cfg.get<double>("Mutation.Add Node");
   result.C1_C2 = cfg.get<double>("Speciation.C1/C2");
   result.C3 = cfg.get<double>("Speciation.C3");
   result.compatibilityFactor = cfg.get<double>("Speciation.Compatibility Factor");
   result.inheritDisabledChance = cfg.get<double>("Mutation.Inherit Disabled");
   result.population = cfg.get<unsigned int>("Basic.Population");
   result.interspecieCrossoverPercentage = cfg.get<double>("Speciation.Interspecie Crossover");
   result.numThreads = cfg.get<unsigned int>("Basic.Threads");
   result.perturbationChance = cfg.get<double>("Mutation.Perturbation");
   result.removeConnectionMutationChance = cfg.get<double>("Mutation.Remove Connection");
   result.weightsMutationChance = cfg.get<double>("Mutation.Weights");

   return result;
}

NeatProject::NeatProject(const boost::property_tree::ptree& cfg, IPlayground& pg)
: mNeat(toNeatConfig(cfg, pg.getNumInputs(), pg.getNumOutputs()), pg.getFitnessEvaluator())
, mConfig(cfg)
, mPlayground(pg)
{

}

void NeatProject::step()
{
   mNeat.step();
   mGeneration++;
}

const neat::Population& NeatProject::getPopulation() const
{
   static neat::Population empty(0, 0, 0);
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
   mNeat.reconfigure(toNeatConfig(mConfig, 0, 0));
}

const unsigned int NeatProject::getAutosavePeriod() const
{
   return mConfig.get<unsigned int>("Basic.Autosave Period");
}

void NeatProject::play(const neat::Genom& g)
{
   mPlayground.play(g);
}