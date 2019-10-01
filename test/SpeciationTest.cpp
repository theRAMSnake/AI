#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "neat/genom.hpp"

class SpeciationTest
{
public:

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