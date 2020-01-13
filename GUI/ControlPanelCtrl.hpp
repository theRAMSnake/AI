#pragma once
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/group.hpp>
#include "widgets/propertygrid.hpp"

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
   nana::propertygrid mGrid;
};