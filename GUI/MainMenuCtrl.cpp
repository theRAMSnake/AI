#include "MainMenuCtrl.hpp"
#include "Trainer.hpp"
#include "ProjectManager.hpp"
#include <nana/gui/filebox.hpp>

void MainMenuCtrl::saveProject()
{
   nana::filebox fb(mWnd, false);
   fb.add_filter("Snake AI Project", "*.saprj");

   auto items = fb();
   if(!items.empty())
   {
      auto file = items[0];
      mPm.save(file);
   }
}

void MainMenuCtrl::loadProject()
{
   nana::filebox fb(mWnd, true);
   fb.add_filter("Snake AI Project", "*.saprj");
   fb.add_filter("All Files", "*.*");

   auto items = fb();
   if(!items.empty())
   {
      auto file = items[0];
      mPm.load(file);
   }
}

MainMenuCtrl::MainMenuCtrl(nana::menubar& parent, ProjectManager& pm, Trainer& trainer)
: mPm(pm)
, mWnd(parent.handle())
{
   auto& p = parent.push_back("Project");
   p.append("Load", std::bind(&MainMenuCtrl::loadProject, this));
   p.append("Save", std::bind(&MainMenuCtrl::saveProject, this));

   trainer.signalStarted.connect([&](){
      p.enabled(false);
   });

   trainer.signalStopped.connect([&](){
      p.enabled(true);
   });
}
