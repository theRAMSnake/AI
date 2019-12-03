#pragma once
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/treebox.hpp>

class Trainer;
class ProjectManager;
class PopulationPanel
{
public:
   PopulationPanel(nana::panel<true>& parent, ProjectManager& pm, Trainer& trainer);

private:
   void refresh();
   void exportGenomFromTree();

   nana::treebox mTree;
   ProjectManager& mPm;
};