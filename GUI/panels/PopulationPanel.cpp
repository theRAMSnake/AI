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

#include "neuroevolution/neuro_net.hpp"
#include <iostream>
#include <fstream>

void PopulationPanel::refresh()
{
   mTree.clear();
   for(auto& s: mPm.getProject()->getPopulation().getSpecies())
   {
      auto path = "root/" + std::to_string(s.id);
      mTree.insert(path, std::to_string(s.id) + " (" + std::to_string(s.averageFitness) + ")");

      std::vector<std::pair<neuroevolution::Fitness, std::string>> popStrings;
      popStrings.reserve(s.popResults.size());
      for(auto& p: s.popResults)
      {
         auto str = std::to_string(p.fitness) + " - H:" + std::to_string(p.nodeCount) +
               " C:" + std::to_string(p.complexity);
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

   mCtx.append ("Export", std::bind(&PopulationPanel::exportAnn, this));
   mCtx.append ("Play", std::bind(&PopulationPanel::play, this));
   mCtx.append ("Visualize", std::bind(&PopulationPanel::visualize, this));
   mTree.events().mouse_down([&](auto args){

      if(mTree.selected().level() == 3 && !trainer.isRunning())
      {
         nana::menu_popuper(mCtx, mTree, nana::point(args.pos.x, args.pos.y), nana::mouse::right_button )(args);
      }
   });
}

void PopulationPanel::exportAnn()
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
         f.open(file, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);

         mPm.getProject()->getPopulation().createAnn(organism)->toBinaryStream(f);
         f.close();
      }
   }
}

void PopulationPanel::play()
{
   auto& organism = extractPopFromSelected();

   mPm.getProject()->play(*mPm.getProject()->getPopulation().createAnn(organism));
}

void PopulationPanel::visualize()
{
   auto& organism = extractPopFromSelected();

   mNnView.reset(new Nn_view(std::move(mPm.getProject()->getPopulation().createAnn(organism))));
}

const PopResult& PopulationPanel::extractPopFromSelected()
{
   auto specieId = boost::lexical_cast<unsigned int>(mTree.selected().owner().key());
   auto organismIdx = boost::lexical_cast<unsigned int>(mTree.selected().key());

   auto& pops = mPm.getProject()->getPopulation();
   auto specieIter = std::find_if(pops.getSpecies().begin(), pops.getSpecies().end(), [=](auto x){return x.id == specieId;});
   return specieIter->popResults[organismIdx];
}