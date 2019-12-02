#include "ControlPanelCtrl.hpp"
#include "Trainer.hpp"
#include "ProjectManager.hpp"
#include <nana/gui/widgets/button.hpp>
#include "widgets/pgitems.hpp"

std::string formatCfgStr(const std::string& input)
{
   //For now strings containg '.' is considered to be double values.
   auto pos = input.find('.');
   if(pos != std::string::npos)
   {
      auto tmpStr = input.substr(0, pos + 4);
      while(tmpStr.back() == '0')
      {
         tmpStr.pop_back();
      }

      return tmpStr;
   }
   else
   {
      return input;
   }
}

ControlPanelCtrl::ControlPanelCtrl(nana::group& parent, ProjectManager& pm, Trainer& trainer)
: mPm(pm)
, mStartStopBtn(parent)
, mUpdateBtn(parent)
, mGrid(parent)
{
   mStartStopBtn.caption("Start");
   mUpdateBtn.caption("Update");

   nana::place layout(parent);
   layout.div("vert <a weight=10><vert d arrange=[30, 30, 87%] margin=10 gap=10>");
   layout.field("d") << mStartStopBtn << mUpdateBtn << mGrid;
   layout.collocate();

   mStartStopBtn.events().click([&](auto args){

      if(!trainer.isRunning())
      {
         mStartStopBtn.caption("Stop");
         trainer.start(pm.getProject());
      }
      else
      {
         mStartStopBtn.caption("Stopping");
         mStartStopBtn.enabled(false);
         mUpdateBtn.enabled(false);

         trainer.stop();
      }
   });

   mUpdateBtn.events().click([&](auto args){

      auto cfg = mPm.getProject().getConfig();

      for(auto& c: cfg)
      {
         for(auto& item: c.second)
         {
            item.second.put("", mGrid.find(c.first, item.first).value());
         }
      }

      try
      {
         mPm.getProject().updateConfig(cfg);
      }
      catch(const std::exception& e)
      {
         nana::msgbox(parent, e.what());
      }
   });

   trainer.signalStopped.connect([&](){
      mStartStopBtn.enabled(true);
      mUpdateBtn.enabled(true);
      mStartStopBtn.caption("Start");
   });

   for(auto& c: mPm.getProject().getConfig())
   {
      auto cat = mGrid.append(c.first);
      for(auto& item: c.second)
      {
         cat.append(nana::propertygrid::pgitem_ptr(new nana::pg_string(item.first, formatCfgStr(item.second.get<std::string>("")))));
      }
   }

   mPm.signalProjectChanged.connect(std::bind(&ControlPanelCtrl::fillGrid, this));
}

void ControlPanelCtrl::fillGrid()
{
   auto cfg = mPm.getProject().getConfig();

   for(auto& c: cfg)
   {
      for(auto& item: c.second)
      {
         mGrid.find(c.first, item.first).value(formatCfgStr(item.second.get<std::string>("")));
      }
   }
}

   