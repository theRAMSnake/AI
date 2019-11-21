#pragma once
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/group.hpp>

class Trainer;
class ProjectManager;
class ControlPanelCtrl
{
public:
   ControlPanelCtrl(nana::group& parent, ProjectManager& pm, Trainer& trainer);

private:
   nana::button mBtn;
};