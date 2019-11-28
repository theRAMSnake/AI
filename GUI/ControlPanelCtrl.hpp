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
   void fillGrid();

   ProjectManager& mPm;
   nana::button mBtn;
   nana::propertygrid mGrid;
};