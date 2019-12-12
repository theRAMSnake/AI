#pragma once
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/treebox.hpp>
#include <nana/gui/widgets/menu.hpp>
#include <neat/population.hpp>
#include "../widgets/nn_view.hpp"

class Trainer;
class ProjectManager;

class PopulationPanel
{
public:
   PopulationPanel(nana::panel<true>& parent, ProjectManager& pm, Trainer& trainer);

private:
   const neat::Pop& extractPopFromSelected();

   void refresh();
   void exportGenomFromTree();
   void playGenom();
   void visualizeGenom();

   nana::treebox mTree;
   ProjectManager& mPm;
   nana::menu mCtx;
   std::unique_ptr<Nn_view> mNnView;
};