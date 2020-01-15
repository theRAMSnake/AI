#include "MainMenuCtrl.hpp"
#include "Trainer.hpp"
#include "ProjectManager.hpp"
#include <nana/gui.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/button.hpp>
#ifdef WIN32
#include <filesystem>
#endif
#include <nana/gui/filebox.hpp>
#include <iostream>
#include <fstream>

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

void MainMenuCtrl::play()
{
   nana::filebox fb(mWnd, true);
   fb.add_filter("ANN", "*.ann");
   fb.add_filter("All Files", "*.*");

   auto items = fb();
   if(!items.empty())
   {
      auto file = items[0];

      std::ifstream f;
      f.open(file, std::ios_base::in | std::ios_base::binary);
      if (f.is_open())
      {
         auto ann = neuroevolution::NeuroNet::fromBinaryStream(f);
         mPm.getProject()->play(*ann);

         f.close();
      }
   }
}

void MainMenuCtrl::newProject()
{
   std::string prjname;

   auto pgs = mPm.getPlaygroundList();
   auto engs = mPm.getEngineList();

   nana::form dlg(mWnd, {250, 160});
   dlg.caption("Choose playground");

   nana::combox cmbPg(dlg);
   nana::combox cmbEng(dlg);
   nana::textbox filename(dlg);
   nana::button btn(dlg);
   btn.caption("OK");

   nana::place layout(dlg);
   layout.div("<vert a arrange=[28, 28, 28, 28] margin=10 gap=5>");
   layout.field("a") << cmbPg << cmbEng << filename << btn;
   layout.collocate();

   for(auto x : pgs)
   {
      cmbPg.push_back(x);
   }

   for(auto x : engs)
   {
      cmbEng.push_back(x);
   }

   btn.events().click.connect([&](){
      dlg.close();
   });

   filename.events().text_changed.connect([&](){
      prjname = filename.text();
   });

   cmbPg.option(0);
   cmbEng.option(0);
   dlg.modality();

   mPm.createProject(pgs[cmbPg.option()], engs[cmbEng.option()], prjname + ".saprj");
}

MainMenuCtrl::MainMenuCtrl(nana::menubar& parent, ProjectManager& pm, Trainer& trainer)
: mPm(pm)
, mWnd(parent.handle())
{
   auto& p = parent.push_back("Project");
   p.append("New", std::bind(&MainMenuCtrl::newProject, this));
   p.append("Load", std::bind(&MainMenuCtrl::loadProject, this));
   auto& saveBtn = p.append("Save", std::bind(&MainMenuCtrl::saveProject, this)).enabled(false);

   auto& t = parent.push_back("Tools");
   auto& playBtn = t.append("Play", std::bind(&MainMenuCtrl::play, this)).enabled(false);

   pm.signalProjectChanged.connect([&](auto& x){
      p.enabled(true);
      saveBtn.enabled(true);
      playBtn.enabled(true);
   });

   trainer.signalStarted.connect([&](){
      p.enabled(false);
   });

   trainer.signalStopped.connect([&](){
      p.enabled(true);
   });
}
