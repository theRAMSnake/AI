#pragma once
#include <string>
#include <memory>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/signals2.hpp>
#include "NeatProject.hpp"
#include "IPlayground.hpp"

class ProjectManager
{
public:
   boost::signals2::signal<void(const NeatProject&)> signalProjectChanged;

   void save(const std::string& fileName);
   bool load(const std::string& fileName);
   void createDefaultProject();
   void createProject(const std::string& playgroundName);
   NeatProject& getProject();
   std::vector<std::string> getPlaygroundList() const;

private:
   IPlayground& createPlayground(const std::string& name);

   std::unique_ptr<IPlayground> mPlayground;
   std::unique_ptr<NeatProject> mCurrentProject;
};