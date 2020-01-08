#pragma once
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/treebox.hpp>
#include <nana/gui/widgets/menu.hpp>
#include "../IPopulation.hpp"
#include "../widgets/nn_view.hpp"

class Trainer;
class ProjectManager;

class PopulationPanel
{
public:
   PopulationPanel(nana::panel<true>& parent, ProjectManager& pm, Trainer& trainer);

private:
   const PopResult& extractPopFromSelected();

   void refresh();
   void exportAnn();
   void play();
   void visualize();

   nana::treebox mTree;
   ProjectManager& mPm;
   nana::menu mCtx;
   std::unique_ptr<Nn_view> mNnView;
};