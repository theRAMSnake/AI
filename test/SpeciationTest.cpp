#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "neat/genom.hpp"
#include "neat/population.hpp"

class SpeciationTest
{
public:
   SpeciationTest()
   {
   }

protected:
   neat::v2::Genom createSampleGenom()
   {
       return neat::v2::Genom::createMinimal(3, 2, mHistory, true);
   }

   neat::InnovationHistory mHistory;
};

BOOST_FIXTURE_TEST_CASE( CalculateDivergence, SpeciationTest ) 
{  
    //Same genom
    neat::v2::Genom a = createSampleGenom();

    BOOST_CHECK_EQUAL(0.0, neat::v2::Genom::calculateDivergence(a, a, 1.0, 2.0));

    neat::v2::Genom b = createSampleGenom();
    neat::v2::mutateAddNode(b, mHistory);

    auto diff = neat::v2::Genom::calculateDivergence(a, b, 1.0, 2.0);
    BOOST_CHECK(diff > 2);

    neat::v2::mutateAddConnection(b, mHistory);
    diff = neat::v2::Genom::calculateDivergence(a, b, 1.0, 2.0);
    BOOST_CHECK(diff > 3);
}

BOOST_FIXTURE_TEST_CASE( Respeciate, SpeciationTest ) 
{
   auto g1 = createSampleGenom();

   auto g2 = g1;
   neat::v2::mutateAddNode(g2, mHistory);

   auto g3 = g2;
   neat::v2::mutateAddNode(g3, mHistory);

   auto g4 = g3;
   neat::v2::mutateAddNode(g4, mHistory);

   auto g5 = g3;
   neat::v2::mutateAddNode(g5, mHistory);

   std::vector<neat::Specie> species { neat::Specie{ neat::Pop{0, g1}, 0}, neat::Specie{ neat::Pop{0, g3}, 0} };

   std::vector<neat::v2::Genom> genoms;
   for(int i = 0; i < 5; ++i)
   {
      genoms.push_back(g1);
   }

   genoms.push_back(g4);
   genoms.push_back(g2);

   neat::v2::mutateWeights(g2);
   genoms.push_back(g2);

   genoms.push_back(g5);
   genoms.push_back(g5);

   neat::Speciation::respeciate(species, genoms, 1.05);

   BOOST_CHECK_EQUAL(3, species.size());
   
   for(auto &s: species)
   {
      for(auto &p: s.population)
      {
         BOOST_CHECK(1.06 > neat::v2::Genom::calculateDivergence(s.representor->genotype, p.genotype, 1.0, 2.0));
      }
   }
}