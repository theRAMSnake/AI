#include "ControlPanelCtrl.hpp"
#include "Trainer.hpp"
#include "ProjectManager.hpp"
#include <nana/gui/widgets/button.hpp>
#include "widgets/pgitems.hpp"

ControlPanelCtrl::ControlPanelCtrl(nana::group& parent, ProjectManager& pm, Trainer& trainer)
: mPm(pm)
, mBtn(parent)
, mGrid(parent)
{
   mBtn.caption("Start");

   nana::place layout(parent);
   layout.div("vert <a weight=10><vert d arrange=[30, 90%] margin=10 gap=10>");
   layout.field("d") << mBtn << mGrid;
   layout.collocate();

   mBtn.events().click([&](auto args){

      if(!trainer.isRunning())
      {
         mBtn.caption("Stop");
         trainer.start(pm.getProject());
      }
      else
      {
         mBtn.caption("Stopping");
         mBtn.enabled(false);

         trainer.stop();
      }
   });

   trainer.signalStopped.connect([&](){
      mBtn.enabled(true);
      mBtn.caption("Start");
   });

   //mPm.signalProjectChanged.connect(std::bind(&ControlPanelCtrl::fillGrid, this));
}

void ControlPanelCtrl::fillGrid()
{
   auto cfg = mPm.getProject().getConfig();

   mGrid.clear();
   mGrid.auto_draw(false);

   //auto cat = mGrid.append("Basic");
   //auto item = cat.append(nana::propertygrid::pgitem_ptr(new nana::pg_string_uint("Population", "800")));
   //cat.append(nana::propertygrid::pgitem_ptr(new nana::pg_string_uint("Num Threads", "6")));
   /*cat.append(nana::propertygrid::pgitem_ptr(new nana::pg_string_uint("Autosave Period", "500")));*/

   //cat = mGrid.append("Speciation");
   //cat.append(nana::propertygrid::pgitem_ptr(new nana::pg_string("Compatibility Factor", "3.0")));
   /*cat.append(nana::propertygrid::pgitem_ptr(new nana::pg_string("C1/C2", "1.0")));
   cat.append(nana::propertygrid::pgitem_ptr(new nana::pg_string("C3", "0.3")));

   cat = mGrid.append("Mutation");
   cat.append(nana::propertygrid::pgitem_ptr(new nana::pg_string("Perturbation", "0.9")));
   cat.append(nana::propertygrid::pgitem_ptr(new nana::pg_string("Add Node", "0.05")));
   cat.append(nana::propertygrid::pgitem_ptr(new nana::pg_string("Add Connection", "0.1")));
   cat.append(nana::propertygrid::pgitem_ptr(new nana::pg_string("Remove Connection", "0.1")));
   cat.append(nana::propertygrid::pgitem_ptr(new nana::pg_string("Weights", "0.8")));*/
}

   