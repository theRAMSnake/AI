#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "neat/genom.hpp"

class SpeciationTest
{
public:
   SpeciationTest()
   {
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
   neat::Genom createSampleGenom()
   {
       return neat::Genom::createMinimal(3, 2, mHistory);
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
    std::cout << diff << std::endl;
    BOOST_CHECK(diff > 2);

    neat::mutateAddConnection(b, mHistory);
    diff = neat::Genom::calculateDivergence(a, b);
    std::cout << diff << std::endl;
    BOOST_CHECK(diff > 3);
}