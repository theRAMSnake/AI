#pragma once

nana::label l(grpCtrl);
   l.caption("Playground:");

   nana::combox c(grpCtrl);

   for(auto x : g_playgrounds)
   {
      c.push_back(x->getName());
   }

   c.events().selected([&](auto args){
      g_neatController.reset(new NeatController(*g_playgrounds[c.option()]));
   });

   c.option(0);

   nana::button b(grpCtrl);
   b.caption("Start");

   b.events().click([&](auto args){

      if(!g_trainer.isRunning())
      {
         if(g_neatController)
         {
            c.enabled(false);
            p.enabled(0, false);
            p.enabled(1, false);
            b.caption("Stop");

            g_trainer.start(*g_neatController);
         }
         else
         {
            (nana::msgbox(fm, "Error").icon(nana::msgbox::icon_error)<<"Please select playground").show();
         }
      }
      else
      {
         b.caption("Stopping");
         b.enabled(false);

         g_trainer.stop();
      }

   });

   g_trainer.onStoped.connect([&](){
      b.enabled(true);
      c.enabled(true);
      p.enabled(0, true);
      p.enabled(1, true);
      b.caption("Start");
   });

   nana::place layout2(grpCtrl);
   layout2.div("vert <a weight=10><vert d arrange=[30,30,30] margin=10>");
   layout2.field("d") << l << c << b;
   layout2.collocate();