#pragma once
#include <string>
#include <memory>
#include <boost/signals2.hpp>
#include "IProject.hpp"
#include "neuroevolution/IPlayground.hpp"

class ProjectManager
{
public:
   boost::signals2::signal<void(const IProject&)> signalProjectChanged;

   void save(const std::string& fileName);
   void autosave();
   bool load(const std::string& fileName);
   void createProject(const std::string& playgroundName, const Engine engine, const std::string& fileName);
   IProject* getProject();
   std::vector<std::string> getPlaygroundList() const;
   std::vector<Engine> getEngineList() const;

   //To support property_tree static nature workaround
   const boost::property_tree::ptree getConfigTemplate(const Engine engine) const;

private:
   neuroevolution::IPlayground& createPlayground(const std::string& name);

   std::unique_ptr<neuroevolution::IPlayground> mPlayground;
   std::unique_ptr<IProject> mCurrentProject;
   std::string mCurrentProjectFileName;
};