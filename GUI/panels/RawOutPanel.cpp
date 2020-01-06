#include "RawOutPanel.hpp"
#include "../Trainer.hpp"
#include "../ProjectManager.hpp"
#include <numeric>
#include <nana/gui/place.hpp>

void printState(NeatProject& c, std::stringstream& out)
{
   auto& pops = c.getPopulation();

   out << "Generation: " << c.getGeneration() << std::endl;
   out << "Total population: " << pops.size() << std::endl;
   out << "Num species: " << pops.numSpecies() << std::endl;
   out << "Average fitness: " << pops.getAverageFitness() << std::endl;

   std::map<ActivationFunctionType, int> numActivations;
   int totalNodes = 0;

   for(auto& s : pops)
   {
      for(auto& p : s.population)
      {
         for(auto n = p.genotype.beginNodes(neat::v2::Genom::NodeType::Hidden);
            n != p.genotype.endNodes(neat::v2::Genom::NodeType::Hidden);
            ++n)
            {
               numActivations[n->acType]++;
               totalNodes++;
            }
      }
   }

   if(totalNodes != 0)
   {
      for(int i = 0; i < NUM_ACTIVATION_FUNCTION_TYPES; ++i)
      {
         out << "ActivationFunction[" << i << "] = " << 
            (double)numActivations[static_cast<ActivationFunctionType>(i)] * 100 / totalNodes << "%" << std::endl;
      }
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