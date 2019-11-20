#include <nana/gui.hpp>
#include <nana/gui/widgets/group.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/menubar.hpp>
#include <nana/gui/filebox.hpp>
#include <nana/gui/place.hpp>
#include "IPlayground.hpp"
#include "NeatController.hpp"
#include "TetrisPG.hpp"
#include <thread>
#include <iomanip>
#include <boost/signals2.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

void printGenom(const neat::Genom& g, std::stringstream& out, const bool includeWeights = false)
{
   out << "         (";

   if(g.length() < 20)
   {
      for(auto& x : g)
      {
         if(x.enabled)
         {
            if(includeWeights)
            {
               out << x.srcNodeId << "->" << x.dstNodeId << ":" << std::setprecision(2) << x.weight << " ";
            }
            else
            {
               out << x.srcNodeId << "->" << x.dstNodeId << " ";
            }
         }
      }
   }
   else
   {
      out << "H:" << g.getHiddenNodeCount() << " C:" << g.getComplexity();
   }

   out << ")" << std::endl;
}

void printState(NeatController& c, std::stringstream& out)
{
   auto& pops = c.getPopulation();

   out << "Generation: " << c.getGeneration() << std::endl;
   out << "Total population: " << pops.size() << std::endl;
   out << "Num species: " << pops.numSpecies() << std::endl;
   out << "Average fitness: " << pops.getAverageFitness() << std::endl;
   out << "Top 5: " << std::endl;

   std::vector<std::pair<int, std::vector<neat::Pop>::const_iterator>> bestPops;
   for(auto& s : pops)
   {
      auto best = std::max_element(s.population.begin(), s.population.end(), [](auto x, auto y){return x.fitness < y.fitness;});
      if(bestPops.size() == 5)
      {
         auto worstOf5 = std::min_element(bestPops.begin(), bestPops.end(), [](auto x, auto y){return x.second->fitness < y.second->fitness;});
         if(worstOf5->second->fitness < best->fitness)
         {
            (*worstOf5) = std::make_pair(s.id, best);
         }
      }
      else
      {
         bestPops.push_back(std::make_pair(s.id, best));
      }
   }

   for(auto &b : bestPops)
   {
      out << b.first << ": Fitness: " << b.second->fitness << std::endl;
      printGenom(b.second->genotype, out);
   }
}

void saveProject(nana::form& fm)
{
   nana::filebox fb(fm, false);
   fb.add_filter("Snake AI Project", "*.saprj");

   auto items = fb();
   if(!items.empty())
   {
      auto file = items[0];
      if(g_neatController)
      {
         g_projectManager.save(file, *g_neatController);
      }
   }
}

void loadProject(nana::form& fm)
{
   nana::filebox fb(fm, true);
   fb.add_filter("Snake AI Project", "*.saprj");
   fb.add_filter("All Files", "*.*");

   auto items = fb();
   if(!items.empty())
   {
      auto file = items[0];
      if(g_neatController)
      {
         g_projectManager.load(file, *g_neatController);
      }
   }
}

void exportProject(nana::form& fm)
{
   nana::filebox fb(fm, true);
   fb.add_filter("All Files", "*.*");

   auto items = fb();
   if(!items.empty())
   {
      auto file = items[0];
      if(g_neatController)
      {
         g_neatController->loadState(file);
      }
   }
}

int main()
{
   ProjectManager projectManager;
   Trainer trainer;

   projectManager.signalProjectChanged.connect(std::bind(Trainer::onProjectChanged, &trainer, std::placeholders::_1));

   projectManager.createDefaultProject();

   nana::size sz = nana::screen().primary_monitor_size();
   nana::form fm(nana::rectangle{ sz });
   fm.caption("Snake AI Tool");

   //-----------------------------------
   
   nana::menubar menu(fm);

   nana::group grpCtrl(fm);
   grpCtrl.caption("Control Panel");

   nana::group grpOut(fm);

   MainMenuCtrl mainMenuCtrl(menu);
   ControlPanelCtrl controlPanelCtrl(grpCtrl);

   nana::place layout(fm);
   layout.div("vert <a weight=28><b arrange=[20%,80%]>");
   layout.field("a") << menu;
   layout.field("b") << grpCtrl << grpOut;
   layout.collocate();

   nana::place layout3(grpOut);
   layout3.div("vert <dock<A> margin=10><dock<B> margin=10>");
   layout3.dock<nana::textbox>("A", "Raw Out");
   layout3.collocate();

   auto& t = *static_cast<nana::textbox*>(layout3.dock_create("output"));

   t.editable(false);

   g_trainer.onOutput.connect([&](auto s){
      t.caption(s);
   });

   fm.show();

   nana::exec();
}