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

std::unique_ptr<nana::widget> dockCreate(nana::window parent, std::string name)
{
   auto panel = std::make_unique<nana::panel<true>>(parent, true);
   panel->caption(name);

   return panel;
}

int main()
{
   ProjectManager projectManager;
   Trainer trainer;

   projectManager.createDefaultProject();

   nana::size sz = nana::screen().primary_monitor_size();
   nana::form fm(nana::rectangle{ sz });
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
   layoutDocks.dock("B", "History", std::bind(dockCreate, std::placeholders::_1, "History"));
   layoutDocks.dock("B", "Population", std::bind(dockCreate, std::placeholders::_1, "Population"));
   layoutDocks.collocate();

   //---------------------------------------------------------------------------------------------------

   MainMenuCtrl mainMenuCtrl(menu, projectManager, trainer);
   ControlPanelCtrl controlPanelCtrl(grpCtrl, projectManager, trainer);

   RawOutPanel rawOutPanel(*static_cast<nana::panel<true>*>(layoutDocks.dock_create("Raw Out")), projectManager, trainer);
   HistoryPanel historyPanel(*static_cast<nana::panel<true>*>(layoutDocks.dock_create("History")), projectManager, trainer);
   PopulationPanel populationPanel(*static_cast<nana::panel<true>*>(layoutDocks.dock_create("Population")), projectManager, trainer);

   fm.show();

   nana::exec();
}