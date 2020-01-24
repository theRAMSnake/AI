#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "snakega/genom.hpp"
#include "snakega/decoder.hpp"

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

    neuroevolution::DomainGeometry geometry;

   geometry.size = {10, 10, 2};
   for (unsigned int i = 0; i < 2; i++)
   {
      for (unsigned int j = 0; j < 5; j++)
      {
         geometry.inputs.push_back({i, j});
         geometry.outputs.push_back({i, j});
      }
   }

   for(int i = 0; i < 1000; ++i)
   {
      g.mutateStructure({});
      g.mutateParameters({});

      auto nn = snakega::GenomDecoder::decode(geometry, g);
   }
}