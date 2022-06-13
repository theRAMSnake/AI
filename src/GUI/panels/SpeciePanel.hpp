#pragma once
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/group.hpp>
#include <nana/gui/drawing.hpp>
#include <nana/gui/widgets/menu.hpp>
#include "../IPopulation.hpp"
#include "../widgets/nn_view.hpp"
#include <memory>

class Trainer;
class ProjectManager;
class IProject;

class SpecieOverview
{
public:
   SpecieOverview(std::shared_ptr<nana::group> impl);

   void update(const SpecieResults& specie, const unsigned int totalPop, IProject& prj);
   void show();
   void hide();

private:
   void exportAnn();
   void play();
   void visualize();

   IProject* mPrj;
   PopResult mTopPop;
   std::shared_ptr<nana::group> mImpl;
   nana::drawing mDrawer;
   nana::menu mCtx;

   double mBarHeightPercentage = 1.0;
   unsigned int mSpecieId = 0;
   unsigned int mTopFitness = 0;
   unsigned int mComplexity = 0;
   unsigned int mNumNodes = 0;
   std::unique_ptr<Nn_view> mNnView;
};

class SpeciePanel
{
public:
   SpeciePanel(nana::panel<true>& parent, ProjectManager& pm, Trainer& trainer);

private:
   void refresh(std::chrono::milliseconds msecs);

   nana::textbox mInfoTxt;
   nana::group mSpecieGroup;
   ProjectManager& mPm;
   std::vector<std::shared_ptr<SpecieOverview>> mSpecieOverviews;
   
};