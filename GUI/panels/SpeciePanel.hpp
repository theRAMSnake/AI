#pragma once
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/group.hpp>
#include <nana/gui/drawing.hpp>
#include "../IPopulation.hpp"
#include <memory>

class Trainer;
class ProjectManager;

class SpecieOverview
{
public:
   SpecieOverview(std::shared_ptr<nana::group> impl);

   void update(const SpecieResults& specie, const unsigned int totalPop);
   void show();
   void hide();

private:
   std::shared_ptr<nana::group> mImpl;
   nana::drawing mDrawer;

   double mBarHeightPercentage = 1.0;
   unsigned int mSpecieId = 0;
   unsigned int mTopFitness = 0;
   unsigned int mComplexity = 0;
   unsigned int mNumNodes = 0;
};

class SpeciePanel
{
public:
   SpeciePanel(nana::panel<true>& parent, ProjectManager& pm, Trainer& trainer);

private:
   void refresh();

   nana::textbox mInfoTxt;
   nana::group mSpecieGroup;
   ProjectManager& mPm;
   std::vector<std::shared_ptr<SpecieOverview>> mSpecieOverviews;
};