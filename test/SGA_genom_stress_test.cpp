#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "snakega/genom.hpp"

class SGAGenomStressTest
{
public:
   SGAGenomStressTest()
   {
   }

protected:
};

BOOST_FIXTURE_TEST_CASE( SGAGenomStressTest1, SGAGenomStressTest ) 
{
   snakega::Genom g = snakega::Genom::createHalfConnected(10, 10);

   for(int i = 0; i < 1000; ++i)
   {
      g.mutateStructure({});
      g.mutateParameters({});
   }
}