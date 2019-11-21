#pragma once
#include <nana/gui/widgets/menubar.hpp>

class ProjectManager;
class Trainer;
class MainMenuCtrl
{
public:
   MainMenuCtrl(nana::menubar& parent, ProjectManager& pm, Trainer& trainer);

private:
   void saveProject();
   void loadProject();

   ProjectManager& mPm;
   nana::window mWnd;
};