#include "ControlPanelCtrl.hpp"
#include "Trainer.hpp"
#include "ProjectManager.hpp"
#include <nana/gui/widgets/button.hpp>

ControlPanelCtrl::ControlPanelCtrl(nana::group& parent, ProjectManager& pm, Trainer& trainer)
: mBtn(parent)
{
   mBtn.caption("Start");

   nana::place layout(parent);
   layout.div("vert <a weight=10><vert d arrange=[30,30,30] margin=[10, 20, 10, 10]>");
   layout.field("d") << mBtn;
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
}

   