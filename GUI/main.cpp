#include <nana/gui.hpp>
#include <nana/gui/widgets/group.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/menubar.hpp>
#include <nana/gui/filebox.hpp>
#include "IPlayground.hpp"
#include "NeatController.hpp"
#include "TetrisPG.hpp"
#include <thread>
#include <iomanip>
#include <boost/signals2.hpp>

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

private:
   void threadFunc()
   {
      mStop = false;

      while(!mStop)
      {
         mCtrl->step();

         onOutput(".");

         if(mCtrl->getGeneration() % 25 == 0)
         {
            std::stringstream s;
            printState(*mCtrl, s);

            onOutput(s.str());
         }
      }

      onStoped();
   }

   bool mStop = true;
   NeatController* mCtrl;
   std::thread mThread;
};

std::vector<std::shared_ptr<IPlayground>> g_playgrounds;
std::unique_ptr<NeatController> g_neatController;
Trainer g_trainer;

void saveState(nana::form& fm)
{
   nana::filebox fb(fm, false);
   fb.add_filter("Snake AI Project", "*.saprj");

   auto items = fb();
   if(!items.empty())
   {
      auto file = items[0];
      if(g_neatController)
      {
         g_neatController->saveState(file);
      }
   }
}

void loadState(nana::form& fm)
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
   p.append("Load", std::bind(&loadState, fm));
   p.append("Save", std::bind(&saveState, fm));

   //-----------------------------------
   
   nana::group grpCtrl(fm, nana::rectangle(10, 30, fm.size().width / 5, fm.size().height - 100));
   grpCtrl.caption("Control Panel");

   nana::label l(grpCtrl, nana::rectangle(10, 20, grpCtrl.size().width - 20, 20));
   l.caption("Playground:");

   nana::combox c(grpCtrl, nana::rectangle(10, 40, grpCtrl.size().width - 20, 20));

   for(auto x : g_playgrounds)
   {
      c.push_back(x->getName());
   }

   c.events().selected([&](auto args){
      g_neatController.reset(new NeatController(*g_playgrounds[c.option()]));
   });

   nana::button b(grpCtrl, nana::rectangle(10, 70, grpCtrl.size().width - 20, 30));
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

   nana::group grpOut(fm, nana::rectangle(grpCtrl.size().width + 20, 30, fm.size().width / 5 * 4 - 80, 300));
   grpOut.caption("Output");

   nana::textbox t(grpOut, nana::rectangle(10, 20, grpOut.size().width - 20, grpOut.size().height - 30));
   t.editable(false);

   g_trainer.onOutput.connect([&](auto s){
      t.append(s, true);
   });

   fm.show();

   nana::exec();
}