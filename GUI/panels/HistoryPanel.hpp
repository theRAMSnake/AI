#pragma once

panel.caption("History");
panel.bgcolor(nana::colors::white);
nana::plot::plot plot(panel);

nana::plot::trace& t1 = plot.AddRealTimeTrace(500);
t1.color( nana::colors::blue );

g_trainer.onOutput.connect([&](auto s){
   t.caption(s);
   t1.add(g_neatController->getPopulation().getAverageFitness());
});