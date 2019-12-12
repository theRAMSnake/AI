#include "PopulationPanel.hpp"

#ifdef WIN32
#include <filesystem>
#endif

#include <nana/gui/place.hpp>
#include <nana/gui/filebox.hpp>
#include <nana/gui/widgets/menu.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "../widgets/nn_view.hpp"
#include "../Trainer.hpp"
#include "../ProjectManager.hpp"

#include "neat/neuro_net.hpp"
#include <iostream>

void PopulationPanel::refresh()
{
   mTree.clear();
   for(auto& s: mPm.getProject().getPopulation())
   {
      auto path = "root/" + std::to_string(s.id);
      mTree.insert(path, std::to_string(s.id) + " (" + std::to_string(s.getSharedFitness()) + ")");

      std::vector<std::pair<neat::Fitness, std::string>> popStrings;
      popStrings.reserve(s.population.size());
      for(auto& p: s.population)
      {
         auto str = std::to_string(p.fitness) + " - H:" + std::to_string(p.genotype.getNumConnectedHiddenNodes()) +
               " C:" + std::to_string(p.genotype.getComplexity());
         popStrings.push_back(std::make_pair(p.fitness, str));
      }

      int i = 0;
      for(auto& p: popStrings)
      {
         mTree.insert(path + "/" + std::to_string(i), p.second);
         i++;
      }
   }
}

PopulationPanel::PopulationPanel(nana::panel<true>& parent, ProjectManager& pm, Trainer& trainer)
: mTree(parent)
, mPm(pm)
{
   parent.bgcolor(nana::colors::white);

   nana::place layoutPop(parent);
   layoutPop.div("<a>");
   layoutPop.field("a") << mTree;
   layoutPop.collocate();

   trainer.signalStopped.connect(std::bind(&PopulationPanel::refresh, this));
   pm.signalProjectChanged.connect(std::bind(&PopulationPanel::refresh, this));

   mCtx.append ("Export", std::bind(&PopulationPanel::exportGenomFromTree, this));
   mCtx.append ("Play", std::bind(&PopulationPanel::playGenom, this));
   mCtx.append ("Visualize", std::bind(&PopulationPanel::visualizeGenom, this));
   mTree.events().mouse_down([&](auto args){

      if(mTree.selected().level() == 3 && !trainer.isRunning())
      {
         nana::menu_popuper(mCtx, mTree, nana::point(args.pos.x, args.pos.y), nana::mouse::right_button )(args);
      }
   });
}

void PopulationPanel::exportGenomFromTree()
{  
   auto& organism = extractPopFromSelected();

   {
      nana::filebox fb(mTree, false);
      fb.add_filter("Genom file", "*.genom");

      auto items = fb();
      if(!items.empty())
      {
         auto file = items[0];
        
         std::ofstream f;
         f.open(file, std::ios_base::out | std::ios_base::trunc);
         for(auto& x : organism.genotype)
         {
            f << x.innovationNumber << ":" << (x.enabled ? "+" : "-") << " " << x.srcNodeId << "->" << x.dstNodeId << " " << x.weight << std::endl;
         }
         
         f.close();
      }
   }
}

void PopulationPanel::playGenom()
{
   auto& organism = extractPopFromSelected();

   mPm.getProject().play(organism.genotype);
}

void PopulationPanel::visualizeGenom()
{
   auto& organism = extractPopFromSelected();

   mNnView.reset(new Nn_view(std::move(std::make_unique<neat::NeuroNet>(organism.genotype))));
}

const neat::Pop& PopulationPanel::extractPopFromSelected()
{
   auto specieId = boost::lexical_cast<unsigned int>(mTree.selected().owner().key());
   auto organismIdx = boost::lexical_cast<unsigned int>(mTree.selected().key());

   auto& pops = mPm.getProject().getPopulation();
   auto specieIter = std::find_if(pops.begin(), pops.end(), [=](auto x){return x.id == specieId;});
   return specieIter->population[organismIdx];
}