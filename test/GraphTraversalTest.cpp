#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "seg/graph_traverser.hpp"

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

BOOST_FIXTURE_TEST_CASE( RandomGraphStability, GraphTraverserTest ) 
{  
   seg::GraphTraverser t(mGraph, mMem);

   for(int i = 0; i < 50; ++i)
   {
      BOOST_CHECK(t.traverseOnce(mInput) >= 0);
      BOOST_CHECK(t.traverseOnce(mInput) < 5);
   }
}

BOOST_FIXTURE_TEST_CASE( RandomWeightedStability, GraphTraverserTest ) 
{  
   seg::GraphTraverser t(mGraph, mMem);

   auto& ch = std::get<seg::Choise>(mGraph.root().payload);
   ch.selector = seg::RandomWeighted {{0.25, 0.55, 0.75, 0.25, 0.25}};

   for(int i = 0; i < 50; ++i)
   {
      BOOST_CHECK(t.traverseOnce(mInput) >= 0);
      BOOST_CHECK(t.traverseOnce(mInput) < 5);
   }
}

BOOST_FIXTURE_TEST_CASE( If, GraphTraverserTest ) 
{  
   seg::GraphTraverser t(mGraph, mMem);

   auto& ch = std::get<seg::Choise>(mGraph.root().payload);

   ch.selector = seg::If {seg::VAL{0.5}, seg::VAL{0.8}, seg::IfSign::Less};
   BOOST_CHECK_EQUAL(0, t.traverseOnce(mInput));

   ch.selector = seg::If {seg::VAL{0.5}, seg::VAL{0.8}, seg::IfSign::Greater};
   BOOST_CHECK_EQUAL(1, t.traverseOnce(mInput));

   ch.selector = seg::If {seg::VAL{0.5}, seg::VAL{0.8}, seg::IfSign::Equals};
   BOOST_CHECK_EQUAL(1, t.traverseOnce(mInput));

   ch.selector = seg::If {seg::VAL{0.5}, seg::VAL{0.8}, seg::IfSign::NotEquals};
   BOOST_CHECK_EQUAL(0, t.traverseOnce(mInput));
}

BOOST_FIXTURE_TEST_CASE( Compare, GraphTraverserTest ) 
{  
   seg::GraphTraverser t(mGraph, mMem);

   auto& ch = std::get<seg::Choise>(mGraph.root().payload);

   ch.selector = seg::Compare {seg::VAL{0.5}, seg::VAL{0.8}};
   BOOST_CHECK_EQUAL(1, t.traverseOnce(mInput));

   ch.selector = seg::Compare {seg::VAL{0.8}, seg::VAL{0.5}};
   BOOST_CHECK_EQUAL(0, t.traverseOnce(mInput));

   ch.selector = seg::Compare {seg::VAL{0.5}, seg::VAL{0.5}};
   BOOST_CHECK_EQUAL(2, t.traverseOnce(mInput));
}

BOOST_FIXTURE_TEST_CASE( Switch, GraphTraverserTest ) 
{  
   seg::GraphTraverser t(mGraph, mMem);

   auto& ch = std::get<seg::Choise>(mGraph.root().payload);

   ch.selector = seg::Switch {seg::VAL{0.5}, {0.25, 0.55, 0.75, 0.25, 0.25}};
   BOOST_CHECK_EQUAL(1, t.traverseOnce(mInput));

   ch.selector = seg::Switch {seg::VAL{0.9}, {0.25, 0.55, 0.75, 0.25, 0.25}};
   BOOST_CHECK_EQUAL(2, t.traverseOnce(mInput));
}

BOOST_FIXTURE_TEST_CASE( Operation, GraphTraverserTest ) 
{  
   seg::GraphTraverser t(mGraph, mMem);

   auto& ch = std::get<seg::Choise>(mGraph.root().payload);
   ch.selector = seg::If {seg::MemAddress{0}, seg::VAL{5.0}, seg::IfSign::Less};
   
   seg::FUNC f{seg::FuncCategory::Primitive, 0, {seg::VAL{1.0}, seg::MemAddress{0}}}; //*0 + 1
   mGraph.get(1).payload = seg::Action{seg::Operation {seg::MemAddress{0}, f, 0}}; //*0 = *0 + 1 -> goto root

   BOOST_CHECK_EQUAL(1, t.traverseOnce(mInput));
   BOOST_CHECK_EQUAL(5.0, mMem.at(0));
}