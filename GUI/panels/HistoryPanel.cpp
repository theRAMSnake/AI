#include "HistoryPanel.hpp"
#include "../widgets/plot.h"
#include "../Trainer.hpp"
#include "../ProjectManager.hpp"

HistoryPanel::HistoryPanel(nana::panel<true>& parent, ProjectManager& pm, Trainer& trainer)
{
   parent.bgcolor(nana::colors::white);
   mPlot = std::make_unique<nana::plot::plot>(parent);

   nana::plot::trace& t1 = mPlot->AddRealTimeTrace(500);
   t1.color( nana::colors::blue );

   trainer.signalStep.connect([&](){
      t1.add(pm.getProject().getPopulation().getAverageFitness());
   });
}