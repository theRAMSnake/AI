#pragma once

class ProjectManager
{
public:
   void save(const std::string& fileName, NeatController& ctrl)
   {
      boost::property_tree::ptree tree;
      tree.put("neat_state_filename", fileName + ".neat");
      tree.put("generation", ctrl.getGeneration());

      boost::property_tree::write_json(fileName, tree);

      ctrl.saveState(fileName + ".neat");
   }

   bool load(const std::string& fileName, NeatController& ctrl)
   {
      try
      {
         boost::property_tree::ptree tree;
         boost::property_tree::read_json(fileName, tree);

         auto neatFileName = tree.get<std::string>("neat_state_filename");

         ctrl.loadState(neatFileName);
         ctrl.setGeneration(tree.get<unsigned int>("generation"));

         return true;
      }
      catch(...)
      {
         return false;
      }
   }
};