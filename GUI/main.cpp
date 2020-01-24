#include "ProjectManager.hpp"
#include "Trainer.hpp"
#include <nana/gui/place.hpp>
#include <nana/gui/screen.hpp>
#include <nana/gui/widgets/form.hpp>
#include <nana/gui/widgets/group.hpp>
#include <nana/gui/widgets/menubar.hpp>
#include "MainMenuCtrl.hpp"
#include "ControlPanelCtrl.hpp"
#include "panels/RawOutPanel.hpp"
#include "panels/HistoryPanel.hpp"
#include "panels/PopulationPanel.hpp"
#include "panels/SpeciePanel.hpp"
#include "logger/Logger.hpp"

std::unique_ptr<nana::widget> dockCreate(nana::window parent, std::string name)
{
   auto panel = std::make_unique<nana::panel<true>>(parent);
   panel->caption(name);

   return panel;
}

int main()
{
   ProjectManager projectManager;
   Trainer trainer;

   trainer.signalStopped.connect(std::bind(&ProjectManager::autosave, &projectManager));

   nana::size sz = nana::screen().primary_monitor_size();
#ifdef WINDOWS
   nana::form fm(nana::rectangle{sz});
#else
   sz.width -= 50;
   nana::form fm(nana::rectangle{sz});
#endif

   fm.caption("Snake AI Tool");
   //---------------------------------------------------------------------------------------------------
   
   nana::menubar menu(fm);

   nana::group grpCtrl(fm);
   grpCtrl.caption("Control Panel");

   nana::group grpOut(fm);

   nana::place layout(fm);
   layout.div("vert <a weight=28><b arrange=[20%,80%]>");
   layout.field("a") << menu;
   layout.field("b") << grpCtrl << grpOut;
   layout.collocate();

   nana::place layoutDocks(grpOut);
   layoutDocks.div("vert <dock<A> margin=10><dock<B> margin=10>");
   layoutDocks.dock("A", "Raw Out", std::bind(dockCreate, std::placeholders::_1, "Raw Out"));
   layoutDocks.dock("A", "Species", std::bind(dockCreate, std::placeholders::_1, "Species"));
   layoutDocks.dock("B", "History", std::bind(dockCreate, std::placeholders::_1, "History"));
   layoutDocks.dock("B", "Population", std::bind(dockCreate, std::placeholders::_1, "Population"));
   layoutDocks.collocate();

   //---------------------------------------------------------------------------------------------------

   MainMenuCtrl mainMenuCtrl(menu, projectManager, trainer);
   ControlPanelCtrl controlPanelCtrl(grpCtrl, projectManager, trainer);

   RawOutPanel rawOutPanel(*static_cast<nana::panel<true>*>(layoutDocks.dock_create("Raw Out")), projectManager, trainer);
   SpeciePanel speciePanel(*static_cast<nana::panel<true>*>(layoutDocks.dock_create("Species")), projectManager, trainer);
   HistoryPanel historyPanel(*static_cast<nana::panel<true>*>(layoutDocks.dock_create("History")), projectManager, trainer);
   PopulationPanel populationPanel(*static_cast<nana::panel<true>*>(layoutDocks.dock_create("Population")), projectManager, trainer);

   //---------------------------------------------------------------------------------------------------

   trainer.signalStep.connect([&](auto x){
      if(projectManager.getProject()->getGeneration() % projectManager.getProject()->getAutosavePeriod() == 0)
      {
         projectManager.autosave();
      }
   });

   fm.events().unload([&](const nana::arg_unload& arg){
      arg.cancel = trainer.isRunning();
   });

   fm.show();

   nana::exec();
}