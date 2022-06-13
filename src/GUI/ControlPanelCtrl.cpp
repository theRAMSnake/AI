#include "ControlPanelCtrl.hpp"
#include "Trainer.hpp"
#include "ProjectManager.hpp"
#include <nana/gui/widgets/button.hpp>
#include "widgets/pgitems.hpp"
#include <iostream>
#include <boost/property_tree/json_parser.hpp>

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
, mGridGrp(parent.handle())
{
   mStartStopBtn.caption("Start");
   mStartStopBtn.enabled(false);
   mUpdateBtn.caption("Update");
   mUpdateBtn.enabled(false);

   nana::place layout(parent);
   layout.div("vert <a weight=10><vert d arrange=[30, 30, 87%] margin=10 gap=10>");
   layout.field("d") << mStartStopBtn << mUpdateBtn << mGridGrp;
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
            item.second.put("", mEngineToGrids[mPm.getProject().getEngine()]->find(c.first, item.first).value());
         }
      }

      mPm.getProject().updateConfig(cfg);
   });

   trainer.signalStopped.connect([&](){
      mStartStopBtn.enabled(true);
      mUpdateBtn.enabled(true);
      mStartStopBtn.caption("Start");
   });

   mPm.signalProjectChanged.connect(std::bind(&ControlPanelCtrl::onProjectChanged, this));

   for(auto& e : mPm.getEngineList())
   {
      auto grid = std::make_shared<nana::propertygrid>(mGridGrp, nana::rectangle(mGridGrp.size()));
      grid->hide();

      for(auto& c: mPm.getConfigTemplate(e))
      {
         auto cat = grid->append(c.first);
         for(auto& item: c.second)
         {
            cat.append(nana::propertygrid::pgitem_ptr(new nana::pg_string(item.first, formatCfgStr(item.second.get<std::string>("")))));
         }
      }

      mEngineToGrids[e] = grid;
   }
}

void ControlPanelCtrl::onProjectChanged()
{
   mStartStopBtn.enabled(true);
   mUpdateBtn.enabled(true);

   for(auto& g : mEngineToGrids)
   {
      g.second->hide();

      if(g.first == mPm.getProject().getEngine())
      {
         g.second->show();

         for(auto& c: mPm.getProject().getConfig())
         {
            for(auto& item: c.second)
            {
               g.second->find(c.first, item.first).value(formatCfgStr(item.second.get<std::string>("")));
            }
         }
      }
   }
}
