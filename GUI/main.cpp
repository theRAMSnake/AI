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

   g_trainer.onStoped.connect([&](){
      b.enabled(true);
      c.enabled(true);
      p.enabled(0, true);
      p.enabled(1, true);
      b.caption("Start");
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
   layout3.collocate();

   auto& t = *static_cast<nana::textbox*>(layout3.dock_create("output"));

   t.editable(false);

   g_trainer.onOutput.connect([&](auto s){
      t.caption(s);
   });

   fm.show();

   nana::exec();
}