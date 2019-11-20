#pragma once

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