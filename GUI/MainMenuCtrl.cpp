#include "MainMenuCtrl.hpp"
#include "Trainer.hpp"
#include "ProjectManager.hpp"
#include <nana/gui.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/button.hpp>
#include <filesystem>
#include <nana/gui/filebox.hpp>
#include <iostream>

void MainMenuCtrl::saveProject()
{
   nana::filebox fb(mWnd, false);
   fb.add_filter("Snake AI Project", "*.saprj");

   auto items = fb();
   if(!items.empty())
   {
      auto file = items[0];
      mPm.save(file.string());
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
      mPm.load(file.string());
   }
}

void MainMenuCtrl::newProject()
{
   std::string prjname;

   auto pgs = mPm.getPlaygroundList();

   nana::form dlg(mWnd, {250, 160});
   dlg.caption("Choose playground");

   nana::combox cmb(dlg);
   nana::textbox filename(dlg);
   nana::button btn(dlg);
   btn.caption("OK");

   nana::place layout(dlg);
   layout.div("<vert a arrange=[28, 28, 28] margin=10 gap=5>");
   layout.field("a") << cmb << filename << btn;
   layout.collocate();

   for(auto x : pgs)
   {
      cmb.push_back(x);
   }

   btn.events().click.connect([&](){
      dlg.close();
   });

   filename.events().text_changed.connect([&](){
      prjname = filename.text();
   });

   cmb.option(0);
   dlg.modality();

   mPm.createProject(pgs[cmb.option()], prjname + ".saprj");
}

MainMenuCtrl::MainMenuCtrl(nana::menubar& parent, ProjectManager& pm, Trainer& trainer)
: mPm(pm)
, mWnd(parent.handle())
{
   auto& p = parent.push_back("Project");
   p.append("New", std::bind(&MainMenuCtrl::newProject, this));
   p.append("Load", std::bind(&MainMenuCtrl::loadProject, this));
   p.append("Save", std::bind(&MainMenuCtrl::saveProject, this));

   trainer.signalStarted.connect([&](){
      p.enabled(false);
   });

   trainer.signalStopped.connect([&](){
      p.enabled(true);
   });
}
