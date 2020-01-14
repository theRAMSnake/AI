#include "ProjectManager.hpp"
#include "TetrisPG.hpp"
#include "CheckpointPG.hpp"
#include "LinesPG.hpp"
#include "projects/NeatProject.hpp"
#include <filesystem>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

IProject* instantiateProject(const boost::property_tree::ptree& cfg, const std::string& engine, neuroevolution::IPlayground& pg)
{
   if(engine == "Neat")
   {
      return new NeatProject(cfg, pg);
   }

   throw -1;
}

void ProjectManager::save(const std::string& fileName)
{
   if(mCurrentProject)
   {
      boost::property_tree::ptree tree = mCurrentProject->getConfig();
      tree.put("neat_state_filename", fileName + ".neat");
      tree.put("generation", mCurrentProject->getGeneration());
      tree.put("playground", mPlayground->getName());
      tree.put("engine", mCurrentProject->getEngine());

      boost::property_tree::write_json(fileName, tree);

      mCurrentProject->saveState(fileName + ".neat");
      mCurrentProjectFileName = fileName;
   }
}

void initiatializeConfig(boost::property_tree::ptree& cfg)
{
   cfg.put("Basic.Population", 1000);
   cfg.put("Basic.Threads", 3);
   cfg.put("Basic.Autosave Period", 500);
   cfg.put("Speciation.Compatibility Factor", 3.0);
   cfg.put("Speciation.C1/C2", 1.0);
   cfg.put("Speciation.C3", 0.3);
   cfg.put("Speciation.Interspecie Crossover", 1);
   cfg.put("Mutation.Perturbation", 0.9);
   cfg.put("Mutation.Add Node", 0.05);
   cfg.put("Mutation.Change Node", 0.2);
   cfg.put("Mutation.Remove Node", 0.05);
   cfg.put("Mutation.Add Connection", 0.1);
   cfg.put("Mutation.Remove Connection", 0.1);
   cfg.put("Mutation.Weights", 0.8);
   cfg.put("Mutation.Inherit Disabled", 0.5);
   cfg.put("Es.Phasing", 0);
}

void checkAndUpdateLegacyProjectFile(boost::property_tree::ptree& cfg)
{
   if(!cfg.get_optional<unsigned int>("Basic.Population"))
   {
      initiatializeConfig(cfg);
   }

   if(!cfg.get_optional<double>("Mutation.Change Node"))
   {
      cfg.put("Mutation.Change Node", 0.2);
   }

   if(!cfg.get_optional<double>("Mutation.Remove Node"))
   {
      cfg.put("Mutation.Remove Node", 0.05);
   }

   if(!cfg.get_optional<int>("Es.Phasing"))
   {
      cfg.put("Es.Phasing", 0);
   }
}

bool ProjectManager::load(const std::string& fileName)
{
    boost::property_tree::ptree tree;
    boost::property_tree::read_json(fileName, tree);

    checkAndUpdateLegacyProjectFile(tree);

    auto neatFileName = tree.get<std::string>("neat_state_filename");

    mCurrentProject.reset(instantiateProject(tree, tree.get<std::string>("engine"), createPlayground(tree.get<std::string>("playground"))));
    mCurrentProject->loadState(neatFileName);
    mCurrentProject->setGeneration(tree.get<unsigned int>("generation"));
    mCurrentProjectFileName = fileName;

    signalProjectChanged(*mCurrentProject);
    return true;
}

void ProjectManager::createProject(const std::string& playgroundName, const std::string& engineName, const std::string& fileName)
{
   boost::property_tree::ptree cfg;
   initiatializeConfig(cfg);

   mCurrentProjectFileName = fileName;
   mCurrentProject.reset(instantiateProject(cfg, engineName, createPlayground(playgroundName)));
   signalProjectChanged(*mCurrentProject);
}

IProject* ProjectManager::getProject()
{
   return mCurrentProject.get();
}

neuroevolution::IPlayground& ProjectManager::createPlayground(const std::string& name)
{
   if(name == "Tetris")
   {
      mPlayground = std::make_unique<TetrisPG>();
   }
   else if(name == "Checkpoint")
   {
      mPlayground = std::make_unique<CheckpointPG>();
   }
   else if(name == "Lines")
   {
      mPlayground = std::make_unique<LinesPG>();
   }
   else
   {
      throw -1;
   }

   return *mPlayground;
}

std::vector<std::string> ProjectManager::getPlaygroundList() const
{
   return {
      "Tetris",
      "Checkpoint",
      "Lines"
   };
}

std::vector<std::string> ProjectManager::getEngineList() const
{
   return {
      "Neat",
      "HyperNeat"
   };
}

void ProjectManager::autosave()
{
   save(mCurrentProjectFileName);
}

const boost::property_tree::ptree ProjectManager::getConfigTemplate() const
{
   boost::property_tree::ptree cfg;
   initiatializeConfig(cfg);

   return cfg;
}