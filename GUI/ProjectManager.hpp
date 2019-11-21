#pragma once
#include <string>
#include <memory>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/signals2.hpp>
#include "NeatProject.hpp"
#include "TetrisPG.hpp"

class ProjectManager
{
public:
   boost::signals2::signal<void(const NeatProject&)> signalProjectChanged;

   void save(const std::string& fileName)
   {
      if(mCurrentProject)
      {
         boost::property_tree::ptree tree;
         tree.put("neat_state_filename", fileName + ".neat");
         tree.put("generation", mCurrentProject->getGeneration());

         boost::property_tree::write_json(fileName, tree);

         mCurrentProject->saveState(fileName + ".neat");
      }
   }

   bool load(const std::string& fileName)
   {
      mCurrentProject = std::make_unique<NeatProject>(mPlayground);
      try
      {
         boost::property_tree::ptree tree;
         boost::property_tree::read_json(fileName, tree);

         auto neatFileName = tree.get<std::string>("neat_state_filename");

         mCurrentProject->loadState(neatFileName);
         mCurrentProject->setGeneration(tree.get<unsigned int>("generation"));

         signalProjectChanged(*mCurrentProject);

         return true;
      }
      catch(...)
      {
         return false;
      }
   }

   void createDefaultProject()
   {
      mCurrentProject = std::make_unique<NeatProject>(mPlayground);
      signalProjectChanged(*mCurrentProject);
   }

   NeatProject& getProject()
   {
      return *mCurrentProject;
   }

private:
   TetrisPG mPlayground;
   std::unique_ptr<NeatProject> mCurrentProject;
};