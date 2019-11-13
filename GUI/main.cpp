#include <nana/gui.hpp>
#include <nana/gui/widgets/group.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/button.hpp>
#include "IPlayground.hpp"
#include "NeatController.hpp"
#include "TetrisPG.hpp"

std::vector<std::shared_ptr<IPlayground>> g_playgrounds;
std::unique_ptr<NeatController> g_neatController;

int main()
{
   g_playgrounds.push_back(std::make_shared<TetrisPG>());

   nana::size sz = nana::screen().primary_monitor_size();
   nana::form fm(nana::rectangle{ sz });
   
   nana::group g(fm, nana::rectangle(10, 10, fm.size().width / 5, fm.size().height - 100));
   g.caption("Control Panel");

   nana::label l(g, nana::rectangle(10, 20, g.size().width - 20, 20));
   l.caption("Playground:");

   nana::combox c(g, nana::rectangle(10, 40, g.size().width - 20, 20));

   for(auto x : g_playgrounds)
   {
      c.push_back(x->getName());
   }

   c.events().selected([&](auto args){
      g_neatController.reset(new NeatController(*g_playgrounds[c.option()]));
   });

   nana::button b(g, nana::rectangle(10, 70, g.size().width - 20, 30));
   b.caption("Start");

   b.events().click([&](auto args){

      if(g_neatController)
      {
         c.enabled(false);
         b.caption("Stop");
      }
      else
      {
         (nana::msgbox(fm, "Error").icon(nana::msgbox::icon_error)<<"Please select playground").show();
      }

   });

   fm.show();

   nana::exec();
}