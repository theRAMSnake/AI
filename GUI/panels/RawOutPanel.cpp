#include "RawOutPanel.hpp"
#include "../Trainer.hpp"
#include "../ProjectManager.hpp"
#include <numeric>
#include <nana/gui/place.hpp>

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

void printState(NeatProject& c, std::stringstream& out)
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

RawOutPanel::RawOutPanel(nana::window parent, ProjectManager& pm, Trainer& trainer)
: mTextBox(parent)
, mPm(pm)
{
   nana::place layoutPop(parent);
   layoutPop.div("<a>");

   layoutPop.field("a") << mTextBox;
   layoutPop.collocate();

   mTextBox.editable(false);

   pm.signalProjectChanged.connect(std::bind(&RawOutPanel::refresh, this));
   trainer.signalStep.connect(std::bind(&RawOutPanel::refresh, this));
}

void RawOutPanel::refresh()
{
   std::stringstream s;
   printState(mPm.getProject(), s);

   mTextBox.caption(s.str());
}