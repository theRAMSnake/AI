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
    result.mutationCfg.changeNodeMutationChance = cfg.get<double>("Mutation.Change Node");
    result.populationCfg.mC1_C2 = cfg.get<double>("Speciation.C1/C2");
    result.populationCfg.mC3 = cfg.get<double>("Speciation.C3");
    result.populationCfg.mCompatibilityFactor = cfg.get<double>("Speciation.Compatibility Factor");
    result.populationCfg.size = cfg.get<unsigned int>("Basic.Population");
    result.populationCfg.minterspecieCrossoverPercentage = cfg.get<double>("Speciation.Interspecie Crossover");
    result.numThreads = cfg.get<unsigned int>("Basic.Threads");
    
    return result;
}

NeatProject::NeatProject(const boost::property_tree::ptree& cfg, neuroevolution::IPlayground& pg)
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

const IPopulation& NeatProject::getPopulation() const
{
   return mPops;
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

const boost::property_tree::ptree& NeatProject::getConfig() const
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

void NeatProject::play(neuroevolution::NeuroNet& ann)
{
   mPlayground.play(ann);
}

neuroevolution::IPlayground& NeatProject::getPlayground()
{
   return mPlayground;
}

std::string NeatProject::getEngine() const
{
   return "Neat";
}

void NeatProject::getRawOut(std::stringstream& out) const
{
   auto& pops = mNeat.getPopulation();

   out << "Generation: " << getGeneration() << std::endl;
   out << "Total population: " << pops.size() << std::endl;
   out << "Num species: " << pops.numSpecies() << std::endl;
   out << "Average fitness: " << pops.getAverageFitness() << std::endl;

   std::map<ActivationFunctionType, int> numActivations;
   int totalNodes = 0;

   for(auto& s : pops)
   {
      for(auto& p : s.population)
      {
         for(auto n = p.genotype.beginNodes(neat::v2::Genom::NodeType::Hidden);
            n != p.genotype.endNodes(neat::v2::Genom::NodeType::Hidden);
            ++n)
            {
               numActivations[n->acType]++;
               totalNodes++;
            }
      }
   }

   if(totalNodes != 0)
   {
      for(int i = 0; i < NUM_ACTIVATION_FUNCTION_TYPES; ++i)
      {
         out << "ActivationFunction[" << i << "] = " << 
            (double)numActivations[static_cast<ActivationFunctionType>(i)] * 100 / totalNodes << "%" << std::endl;
      }
   }
}