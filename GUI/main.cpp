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

class Trainer
{
public:
   boost::signals2::signal<void()> onStoped;
   boost::signals2::signal<void(std::string)> onOutput;

   void start(NeatController& c)
   {
      mCtrl = &c;
      if(mThread.joinable())
      {
         mThread.join();
      }
      mThread = std::thread(&Trainer::threadFunc, this);
   }

   void stop()
   {
      mStop = true;
   }

   bool isRunning()
   {
      return !mStop;
   }

   ~Trainer()
   {
      if(mThread.joinable())
      {
         mThread.join();
      }
   }

private:
   void threadFunc()
   {
      mStop = false;

      while(!mStop)
      {
         mCtrl->step();

         std::stringstream s;
         printState(*mCtrl, s);

         onOutput(s.str());
      }

      onStoped();
   }

   bool mStop = true;
   NeatController* mCtrl;
   std::thread mThread;
};

class ProjectManager
{
public:
   void save(const std::string& fileName, NeatController& ctrl)
   {
      boost::property_tree::ptree tree;
      tree.put("neat_state_filename", fileName + ".neat");
      tree.put("generation", ctrl.getGeneration());

      boost::property_tree::write_json(fileName, tree);

      ctrl.saveState(fileName + ".neat");
   }

   bool load(const std::string& fileName, NeatController& ctrl)
   {
      try
      {
         boost::property_tree::ptree tree;
         boost::property_tree::read_json(fileName, tree);

         auto neatFileName = tree.get<std::string>("neat_state_filename");

         ctrl.loadState(neatFileName);
         ctrl.setGeneration(tree.get<unsigned int>("generation"));

         return true;
      }
      catch(...)
      {
         return false;
      }
   }
};

std::vector<std::shared_ptr<IPlayground>> g_playgrounds;
std::unique_ptr<NeatController> g_neatController;
Trainer g_trainer;
ProjectManager g_projectManager;

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
   g_playgrounds.push_back(std::make_shared<TetrisPG>());

   nana::size sz = nana::screen().primary_monitor_size();
   nana::form fm(nana::rectangle{ sz });
   fm.caption("Snake AI Tool");

   //-----------------------------------

   nana::menubar menu(fm);
   auto& p = menu.push_back("Project");
   p.append("Load", std::bind(&loadProject, fm));
   p.append("Save", std::bind(&saveProject, fm));
   p.append("Export", std::bind(&exportProject, fm));

   //-----------------------------------
   
   nana::group grpCtrl(fm);
   grpCtrl.caption("Control Panel");

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

   //----------------------------------

   nana::group grpOut(fm);

   //-----------------------------------

   nana::place layout(fm);
   layout.div("vert <a weight=28><b arrange=[20%,80%]>");
   layout.field("a") << menu;
   layout.field("b") << grpCtrl << grpOut;
   layout.collocate();

   nana::place layout2(grpCtrl);
   layout2.div("vert <a weight=10><vert d arrange=[30,30,30] margin=10>");
   layout2.field("d") << l << c << b;
   layout2.collocate();

   nana::place layout3(grpOut);
   layout3.div("vert <dock<A> margin=10><dock<B> margin=10>");
   layout3.dock<nana::textbox>("A", "Raw Out");
   layout3.dock<nana::panel<true>>("B", "History");
   layout3.dock<nana::panel<true>>("B", "Population");
   layout3.collocate();

   auto& t = *static_cast<nana::textbox*>(layout3.dock_create("Raw Out"));
   t.editable(false);

   auto& panel = *static_cast<nana::panel<true>*>(layout3.dock_create("History"));
   panel.caption("History");
   panel.bgcolor(nana::colors::white);
   nana::plot::plot plot(panel);

   nana::plot::trace& t1 = plot.AddRealTimeTrace(500);
   t1.color( nana::colors::blue );

   g_trainer.onOutput.connect([&](auto s){
      t.caption(s);
      t1.add(g_neatController->getPopulation().getAverageFitness());
   });

   auto& popPanel = *static_cast<nana::panel<true>*>(layout3.dock_create("Population"));
   popPanel.caption("Population");
   popPanel.bgcolor(nana::colors::white);

   nana::place layoutPop(popPanel);
   layoutPop.div("<a>");

   nana::treebox popTree(popPanel);
   layoutPop.field("a") << popTree;
   layoutPop.collocate();

   g_trainer.onStoped.connect([&](){
      b.enabled(true);
      c.enabled(true);
      p.enabled(0, true);
      p.enabled(1, true);
      b.caption("Start");

      popTree.clear();
      for(auto& s: g_neatController->getPopulation())
      {
         auto path = "root/" + std::to_string(s.id);
         popTree.insert(path, std::to_string(s.id) + " (" + std::to_string(s.getSharedFitness()) + ")");

         std::vector<std::pair<neat::Fitness, std::string>> popStrings;
         popStrings.reserve(s.population.size());
         for(auto& p: s.population)
         {
            auto str = std::to_string(p.fitness) + " - H:" + std::to_string(p.genotype.getHiddenNodeCount()) +
                " C:" + std::to_string(p.genotype.getComplexity());
            popStrings.push_back(std::make_pair(p.fitness, str));
         }

         std::sort(popStrings.begin(), popStrings.end(), [](auto& x, auto& y){return x.first > y.first;});

         int i = 0;
         for(auto& p: popStrings)
         {
            i++;
            popTree.insert(path + "/" + std::to_string(i), p.second);
         }
      }
   });

   fm.show();

   nana::exec();
}