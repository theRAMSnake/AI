#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "snakega/fitness_weighted_pool.hpp"

class FitnessWeightedPoolTest
{
public:
   FitnessWeightedPoolTest()
   {
   }

protected:
};

BOOST_FIXTURE_TEST_CASE( FitnessWeightedPoolTest1, FitnessWeightedPoolTest ) 
{
   std::vector<snakega::Pop> pops;

   for(int i = 0; i < 10; ++i)
   {
      pops.push_back(snakega::Pop(snakega::Genom(5, 5)));
      pops.back().mFitness = i * 10;
   }

   snakega::FitnessWeightedPool pool(pops.begin(), pops.end(), 90);

   for(int i = 0; i < 10; ++i)
   {
      std::cout << " F: " << pool.pick().mFitness;
   }

   std::cout << std::endl;
}