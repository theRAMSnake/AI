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

unsigned int getRawNumIOs(const std::vector<gacommon::IOElement>& inputs)
{
   unsigned int result = 0;
   for(auto& i : inputs)
   {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr(std::is_same_v<gacommon::ValueIO, T>)
            {
                result++;
            }
            else if constexpr(std::is_same_v<gacommon::ChoiceIO, T>)
            {
                result += arg.options;
            }
            else if constexpr(std::is_same_v<gacommon::BitmapIO, T>)
            {
                result += arg.map.size();
            }
            else
            {
                throw std::runtime_error("Unhandled in run");
            }
        }, i);
   }

   return result;
}

NeatProject::NeatProject(const boost::property_tree::ptree& cfg, gacommon::IPlayground& pg)
: ProjectBase(cfg, pg)
{
   mNeat.reset(new neat::Neat(toNeatConfig(cfg, getRawNumIOs(pg.getInputs()), getRawNumIOs(pg.getOutputs())), 
     cfg.get<int>("Es.Phasing") == 0 ? neat::EvolutionStrategyType::Blend : neat::EvolutionStrategyType::Phasing,
     pg.getFitnessEvaluator()));

   mPops.reset(new NeatPopulation(*mNeat));
}

void NeatProject::step()
{
   getPlayground().step();
   mNeat->step();
   mGeneration++;
}

const IPopulation& NeatProject::getPopulation() const
{
   return *mPops;
}

void NeatProject::saveState(const std::string& filename)
{
   mNeat->saveState(filename);
}

void NeatProject::loadState(const std::string& filename)
{
   mNeat->loadState(filename);
}

void NeatProject::reconfigure()
{
   mNeat->reconfigure(toNeatConfig(getConfig(), 0, 0), 
      getConfig().get<int>("Es.Phasing") == 0 ? neat::EvolutionStrategyType::Blend : neat::EvolutionStrategyType::Phasing);
}

Engine NeatProject::getEngine() const
{
   return Engine::Neat;
}

void NeatProject::getRawOut(std::stringstream& out) const
{
   if(!mNeat->hasPopulation())
   {
      return;
   }
   
   auto& pops = mNeat->getPopulation();

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
