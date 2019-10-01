#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <ctime>
#include "neat/population.hpp"
#include "neat/rng.hpp"

class PopulationTest
{
public:
   PopulationTest()
   {
      neat::Rng::seed(static_cast<unsigned int>(std::time(0)));
   }

protected:
   neat::Population createSamplePopulation()
   {
      neat::Population result;

      for(unsigned int i = 0; i < 3; ++i)
      {
         result += neat::Specie {i, {{neat::Fitness(100 * i), createSampleGenom()}, 
            {neat::Fitness(200 * i), createSampleGenom()}, 
            {neat::Fitness(300 * i), createSampleGenom()}}};
      }

      return result;
   }

   neat::Genom createSampleGenom()
   {
       return neat::Genom::createMinimal(3, 2, mHistory);
   }

   neat::InnovationHistory mHistory;
};

BOOST_FIXTURE_TEST_CASE(createSpecieSamplesTest , PopulationTest) 
{  
   auto population = createSamplePopulation();
   auto samples = population.createSpeciesSamples();

   BOOST_CHECK_EQUAL(population.numSpecies(), samples.size());
   auto pos = samples.begin();

   for(auto s : population)
   {
      //Fitnesses are especialy made distinct
      BOOST_CHECK(std::find_if(s.population.begin(), s.population.end(), [&](auto x){return x.fitness == pos->fitness;}) != s.population.end());
      pos++;
   }
}

BOOST_FIXTURE_TEST_CASE(getSpecieSharedFitnessTest , PopulationTest) 
{  
   auto population = createSamplePopulation();
   auto fitness = population.getSpeciesSharedFitness();

   BOOST_CHECK_EQUAL(0, fitness[0]);
   BOOST_CHECK_EQUAL(200, fitness[1]);
   BOOST_CHECK_EQUAL(400, fitness[2]);
}