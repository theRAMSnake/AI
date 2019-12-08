#include "NeatProject.hpp"

NeatProject::NeatProject(IPlayground& pg)
: mNeat(pg.getConfig(), pg.getFitnessEvaluator())
, mPlayground(pg)
{
   //Current config policy - inherit from playground
   auto pgConfig = pg.getConfig();

   /*mConfig.put("Population", pgConfig.optimalPopulation);
   mConfig.put("Threads", pgConfig.numThreads);
   mConfig.put("Autosave Period", 500);
   mConfig.put("Compatibility Factor", pgConfig.compatibilityFactor);
   mConfig.put("C1/C2", pgConfig.C1_C2);
   mConfig.put("C3", pgConfig.C3);
   mConfig.put("Mutation.Perturbation", pgConfig.perturbationChance);
   mConfig.put("Mutation.Add Node", pgConfig.addNodeMutationChance);
   mConfig.put("Mutation.Add Connection", pgConfig.addConnectionMutationChance);
   mConfig.put("Mutation.Remove Connection", pgConfig.removeConnectionMutationChance);
   mConfig.put("Mutation.Weights", pgConfig.weightsMutationChance);*/
}

void NeatProject::step()
{
   mPlayground.step();
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