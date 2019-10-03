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

      neat::Config c;

      c.numInputs = 3;
      c.numOutputs = 2;
      c.initialPopulation = 100;
      c.optimalPopulation = 100;
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