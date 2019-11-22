#include "PopulationPanel.hpp"
#include <nana/gui/place.hpp>
#include "../Trainer.hpp"
#include "../ProjectManager.hpp"

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
         auto str = std::to_string(p.fitness) + " - H:" + std::to_string(p.genotype.getHiddenNodeCount()) +
               " C:" + std::to_string(p.genotype.getComplexity());
         popStrings.push_back(std::make_pair(p.fitness, str));
      }

      std::sort(popStrings.begin(), popStrings.end(), [](auto& x, auto& y){return x.first > y.first;});

      int i = 0;
      for(auto& p: popStrings)
      {
         i++;
         mTree.insert(path + "/" + std::to_string(i), p.second);
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
}