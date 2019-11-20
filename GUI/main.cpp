#include <nana/gui.hpp>
#include <nana/gui/widgets/group.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/menubar.hpp>
#include <nana/gui/widgets/treebox.hpp>
#include <nana/gui/filebox.hpp>
#include <nana/gui/place.hpp>
#include "IPlayground.hpp"
#include "NeatController.hpp"
#include "TetrisPG.hpp"
#include <thread>
#include <iomanip>
#include <numeric>
#include <boost/signals2.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "widgets/plot.h"

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

   std::vector<std::pair<int, const neat::Pop*>> bestPops;
   for(auto& s : pops)
   {
      for(auto& p : s.population)
      {
         if(bestPops.size() == 10)
         {
            auto worst = std::min_element(bestPops.begin(), bestPops.end(), [](auto x, auto y){return x.second->fitness < y.second->fitness;});
            if(worst->second->fitness < p.fitness)
            {
               (*worst) = std::make_pair(s.id, &p);
            }
         }
         else
         {
            bestPops.push_back(std::make_pair(s.id, &p));
         }
      }
   }

   std::sort(bestPops.begin(), bestPops.end(), [](auto& x, auto& y){return x.second->fitness > y.second->fitness;});

   out << "Top 10 fitness: " << std::accumulate(bestPops.begin(), bestPops.end(), 0, [](auto& x, auto& y){return x + y.second->fitness;}) << std::endl;
   out << "Top 10: " << std::endl;

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
   layoutDocks.dock<nana::panel<true>>("A", "Raw Out", ::nana::string("Raw Out"));
   layoutDocks.dock<nana::panel<true>>("B", "History", ::nana::string("History"));
   layoutDocks.dock<nana::panel<true>>("B", "Population", ::nana::string("Population"));
   layoutDocks.collocate();

   //---------------------------------------------------------------------------------------------------

   MainMenuCtrl mainMenuCtrl(menu);
   ControlPanelCtrl controlPanelCtrl(grpCtrl);

   RawOutPanel rawOutPanel(*static_cast<nana::panel<true>*>(layoutDocks.dock_create("Raw Out")));
   HistoryPanel historyPanel(*static_cast<nana::panel<true>*>(layoutDocks.dock_create("History")));
   PopulationPanel populationPanel(*static_cast<nana::panel<true>*>(layoutDocks.dock_create("Population")));

   fm.show();

   nana::exec();
}