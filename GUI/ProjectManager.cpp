#include "ProjectManager.hpp"
#include "EmptyPG.hpp"
#include "TetrisPG.hpp"
#include "CheckpointPG.hpp"

void ProjectManager::save(const std::string& fileName)
{
   if(mCurrentProject)
   {
      boost::property_tree::ptree tree;
      tree.put("neat_state_filename", fileName + ".neat");
      tree.put("generation", mCurrentProject->getGeneration());
      tree.put("playground", mPlayground->getName());

      boost::property_tree::write_json(fileName, tree);

      mCurrentProject->saveState(fileName + ".neat");
   }
}

bool ProjectManager::load(const std::string& fileName)
{
   try
   {
      boost::property_tree::ptree tree;
      boost::property_tree::read_json(fileName, tree);

      auto neatFileName = tree.get<std::string>("neat_state_filename");

      mCurrentProject = std::make_unique<NeatProject>(createPlayground(tree.get<std::string>("playground")));
      mCurrentProject->loadState(neatFileName);
      mCurrentProject->setGeneration(tree.get<unsigned int>("generation"));

      signalProjectChanged(*mCurrentProject);
      return true;
   }
   catch(...)
   {
      mCurrentProject = std::make_unique<NeatProject>(createPlayground("Empty"));
      return false;
   }
}

void ProjectManager::createDefaultProject()
{
   mCurrentProject = std::make_unique<NeatProject>(createPlayground("Empty"));
   signalProjectChanged(*mCurrentProject);
}

void ProjectManager::createProject(const std::string& playgroundName)
{
   mCurrentProject = std::make_unique<NeatProject>(createPlayground(playgroundName));
   signalProjectChanged(*mCurrentProject);
}

NeatProject& ProjectManager::getProject()
{
   return *mCurrentProject;
}

IPlayground& ProjectManager::createPlayground(const std::string& name)
{
   if(name == "Empty")
   {
      mPlayground = std::make_unique<EmptyPG>();
   }
   else if(name == "Tetris")
   {
      mPlayground = std::make_unique<TetrisPG>();
   }
   else if(name == "Checkpoint")
   {
      mPlayground = std::make_unique<CheckpointPG>();
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
      "Empty",
      "Tetris",
      "Checkpoint"
   };
}