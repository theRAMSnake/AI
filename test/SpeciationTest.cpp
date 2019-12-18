#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "neat/genom.hpp"
#include "neat/population.hpp"

class SpeciationTest
{
public:
   SpeciationTest()
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

BOOST_FIXTURE_TEST_CASE( CalculateDivergence, SpeciationTest ) 
{  
    //Same genom
    neat::Genom a = createSampleGenom();

    BOOST_CHECK_EQUAL(0.0, neat::Genom::calculateDivergence(a, a));

    neat::Genom b = createSampleGenom();
    neat::mutateAddNode(b, mHistory);

    auto diff = neat::Genom::calculateDivergence(a, b);
    BOOST_CHECK(diff > 2);

    neat::mutateAddConnection(b, mHistory);
    diff = neat::Genom::calculateDivergence(a, b);
    BOOST_CHECK(diff > 3);
}

BOOST_FIXTURE_TEST_CASE( Respeciate, SpeciationTest ) 
{
   auto g1 = createSampleGenom();

   auto g2 = g1;
   neat::mutateAddNode(g2, mHistory);

   auto g3 = g2;
   neat::mutateAddNode(g3, mHistory);

   auto g4 = g3;
   neat::mutateAddNode(g4, mHistory);

   auto g5 = g3;
   neat::mutateAddNode(g5, mHistory);

   std::vector<neat::Specie> species { neat::Specie{ neat::Pop{0, g1}, 0}, neat::Specie{ neat::Pop{0, g3}, 0} };

   std::vector<neat::Genom> genoms;
   for(int i = 0; i < 5; ++i)
   {
      genoms.push_back(g1);
   }

   genoms.push_back(g4);
   genoms.push_back(g2);

   neat::mutateWeights(g2);
   genoms.push_back(g2);

   genoms.push_back(g5);
   genoms.push_back(g5);

   neat::Speciation::respeciate(species, genoms, 1.05);

   BOOST_CHECK_EQUAL(3, species.size());
   
   for(auto &s: species)
   {
      for(auto &p: s.population)
      {
         BOOST_CHECK(1.06 > neat::Genom::calculateDivergence(s.representor->genotype, p.genotype));
      }
   }
}