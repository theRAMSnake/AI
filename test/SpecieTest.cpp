#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "neat/population.hpp"

class SpecieTest
{
public:
   SpecieTest()
   {
      neat::Config c;

      c.numInputs = 3;
      c.numOutputs = 2;
      c.population = 100;
      c.compatibilityFactor = 3.0;
      c.inheritDisabledChance = 0.75;
      c.perturbationChance = 0.9;
      c.addNodeMutationChance = 0.05;
      c.addConnectionMutationChance = 0.05;
      c.removeConnectionMutationChance = 0.05;
      c.weightsMutationChance = 0.8;
      c.C1_C2 = 1.0;
      c.C3 = 2.0;

      neat::Genom::setConfig(c);
   }

protected:
   neat::Genom createSampleGenom()
   {
       return neat::Genom::createMinimal(3, 2, mHistory, true);
   }

   neat::InnovationHistory mHistory;
};

BOOST_FIXTURE_TEST_CASE( UpdateFitness, SpecieTest ) 
{
   neat::Specie s;
   s.population.push_back({5, createSampleGenom()});
   s.population.push_back({5, createSampleGenom()});
   s.population.push_back({5, createSampleGenom()});
   s.population.push_back({10, createSampleGenom()});
   s.population.push_back({10, createSampleGenom()});
   s.population.push_back({10, createSampleGenom()});
   s.population.push_back({20, createSampleGenom()});
   s.population.push_back({20, createSampleGenom()});
   s.population.push_back({0, createSampleGenom()});
   s.population.push_back({100, createSampleGenom()});

   s.updateFitness();

   BOOST_CHECK_EQUAL(185, s.getTotalFitness());
   BOOST_CHECK_EQUAL(18, s.getSharedFitness());
}

BOOST_FIXTURE_TEST_CASE( produceOffsprings, SpecieTest ) 
{
   neat::Specie s;

   auto winner = createSampleGenom();
   neat::mutateAddNode(winner, mHistory);

   s.population.push_back({5, createSampleGenom()});
   s.population.push_back({5, createSampleGenom()});
   s.population.push_back({5, createSampleGenom()});
   s.population.push_back({10, winner});
   s.population.push_back({10, winner});
   s.population.push_back({10, winner});
   s.population.push_back({20, winner});
   s.population.push_back({20, winner});
   s.population.push_back({0, createSampleGenom()});

   auto champ = winner;
   neat::mutateAddConnection(champ, mHistory);
   s.population.push_back({100, champ});

   s.updateFitness();

   std::vector<neat::Genom> out;
   s.produceOffsprings(10, mHistory, out);

   BOOST_CHECK_EQUAL(10, out.size());

   //make sure champion is there
   BOOST_CHECK(neat::Genom::calculateDivergence(champ, out[0]) == 0);

   //Check that we do not have offsprings of worst half
   for(auto g : out)
   {
      BOOST_CHECK(g.getHiddenNodeCount() != 0);
   }
}