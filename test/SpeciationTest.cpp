#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "neat/genom.hpp"

class SpeciationTest
{
public:

protected:
   neat::Genom createSampleGenom()
   {
       return neat::Genom::createMinimal(3, 2);
   }

   const neat::InnovationNumber INNITIAL_INNOVATION_NUMBER = 3 * 2;
};

BOOST_FIXTURE_TEST_CASE( CalculateDivergence, SpeciationTest ) 
{  
    //Same genom
    neat::InnovationNumber innovationNumber = INNITIAL_INNOVATION_NUMBER;
    neat::Genom a = createSampleGenom();

    BOOST_CHECK_EQUAL(0.0, neat::Genom::calculateDivergence(a, a));

    neat::Genom b = createSampleGenom();
    neat::mutateAddNode(b, innovationNumber);

    auto diff = neat::Genom::calculateDivergence(a, b);
    std::cout << diff << std::endl;
    BOOST_CHECK(diff > 2);

    neat::mutateAddConnection(b, innovationNumber);
    diff = neat::Genom::calculateDivergence(a, b);
    std::cout << diff << std::endl;
    BOOST_CHECK(diff > 3);
}