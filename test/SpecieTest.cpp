#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "neat/population.hpp"

class SpecieTest
{
public:
   SpecieTest()
   {

   }

protected:
   neat::v2::Genom createSampleGenom()
   {
       return neat::v2::Genom::createMinimal(3, 2, mHistory, true);
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
   neat::v2::mutateAddNode(winner, mHistory);

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
   neat::v2::mutateAddConnection(champ, mHistory);
   s.population.push_back({100, champ});

   s.updateFitness();

   std::vector<neat::v2::Genom> out;
   s.produceOffsprings(10, mHistory, true, {}, out);

   BOOST_CHECK_EQUAL(10, out.size());

   //make sure champion is there
   BOOST_CHECK(neat::v2::Genom::calculateDivergence(champ, out[0], 1.0, 2.0) == 0);

   //Check that we do not have offsprings of worst half
   for(auto g : out)
   {
      BOOST_CHECK(g.getNodeCount(neat::v2::Genom::NodeType::Hidden) != 0);
   }
}