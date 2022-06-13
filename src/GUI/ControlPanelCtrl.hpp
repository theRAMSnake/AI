#pragma once
#include <map>
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/group.hpp>
#include "widgets/propertygrid.hpp"
#include "ProjectManager.hpp"

class Trainer;
class ProjectManager;
class ControlPanelCtrl
{
public:
   ControlPanelCtrl(nana::group& parent, ProjectManager& pm, Trainer& trainer);

private:
   void onProjectChanged();

   ProjectManager& mPm;
   nana::button mStartStopBtn;
   nana::button mUpdateBtn;
   nana::group mGridGrp;
   std::map<Engine, std::shared_ptr<nana::propertygrid>> mEngineToGrids;
};