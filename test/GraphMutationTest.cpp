#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "seg/graph_traverser.hpp"
#include "seg/mutator.hpp"

class GraphTraverserTest
{
public:
   GraphTraverserTest()
   : mMem(16)
   , mGraph(5)
   {
        
   }

protected:
   double mInput[5];
   seg::Memory mMem;
   seg::Graph mGraph;
};

BOOST_FIXTURE_TEST_CASE( Stability, GraphTraverserTest ) 
{  
   seg::GraphTraverser t(mGraph, mMem);

   seg::Mutator m(seg::MutationConfig(), mGraph, 16, 5, 5);

   for(int i = 0; i < 500; ++i)
   {
      m.mutate();
      t.traverseOnce(mInput);
   }
}