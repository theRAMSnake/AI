#include "ProjectManager.hpp"
#include "TetrisPG.hpp"
#include "CheckpointPG.hpp"
#include "LinesPG.hpp"
#include "projects/NeatProject.hpp"
#include "projects/SGAProject.hpp"
#include <filesystem>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

IProject* instantiateProject(const boost::property_tree::ptree& cfg, const Engine engine, neuroevolution::IPlayground& pg)
{
   if(engine == Engine::Neat)
   {
      return new NeatProject(cfg, false, pg);
   }
   else if(engine == Engine::HyperNeat)
   {
      return new NeatProject(cfg, true, pg);
   }
   else if(engine == Engine::SnakeGA)
   {
      return new SGAProject(cfg, pg);
   }

   throw -1;
}

void ProjectManager::save(const std::string& fileName)
{
   if(mCurrentProject)
   {
      boost::property_tree::ptree tree = mCurrentProject->getConfig();
      tree.put("state_filename", fileName + ".state");
      tree.put("generation", mCurrentProject->getGeneration());
      tree.put("playground", mPlayground->getName());
      tree.put("engine", to_string(mCurrentProject->getEngine()));

      boost::property_tree::write_json(fileName, tree);

      mCurrentProject->saveState(fileName + ".state");
      mCurrentProjectFileName = fileName;
   }
}

void initiatializeConfig(const Engine engine, boost::property_tree::ptree& cfg)
{
   cfg.put("Basic.Population", 100);
   cfg.put("Basic.Threads", 3);
   cfg.put("Basic.Autosave Period", 500);

   if(engine == Engine::Neat || engine == Engine::HyperNeat)
   {
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
   else if(engine == Engine::SnakeGA)
   {
      cfg.put("Selection.Champions Kept", 5);
      cfg.put("Selection.Survival Rate", 0.25);
      cfg.put("Exploitation.Depth", 10);
      cfg.put("Exploitation.Size", 25);
   }
   else
   {
      throw -1;
   }
}

bool ProjectManager::load(const std::string& fileName)
{
    boost::property_tree::ptree tree;
    boost::property_tree::read_json(fileName, tree);

    auto neatFileName = tree.get<std::string>("state_filename");

    mCurrentProject.reset(instantiateProject(tree, to_engine(tree.get<std::string>("engine")), createPlayground(tree.get<std::string>("playground"))));
    mCurrentProject->loadState(neatFileName);
    mCurrentProject->setGeneration(tree.get<unsigned int>("generation"));
    mCurrentProjectFileName = fileName;

    signalProjectChanged(*mCurrentProject);
    return true;
}

void ProjectManager::createProject(const std::string& playgroundName, const Engine engine, const std::string& fileName)
{
   boost::property_tree::ptree cfg;
   initiatializeConfig(engine, cfg);

   mCurrentProjectFileName = fileName;
   mCurrentProject.reset(instantiateProject(cfg, engine, createPlayground(playgroundName)));
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

std::vector<Engine> ProjectManager::getEngineList() const
{
   return {
      Engine::Neat,
      Engine::HyperNeat,
      Engine::SnakeGA
   };
}

void ProjectManager::autosave()
{
   save(mCurrentProjectFileName);
}

const boost::property_tree::ptree ProjectManager::getConfigTemplate(const Engine engine) const
{
   boost::property_tree::ptree cfg;
   initiatializeConfig(engine, cfg);

   return cfg;
}