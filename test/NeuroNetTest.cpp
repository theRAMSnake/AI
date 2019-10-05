#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "neat/neuro_net.hpp"

class NeuroNetTest
{
public:
   NeuroNetTest()
   {
      neat::Config c;

      c.numInputs = 2;
      c.numOutputs = 1;
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
       return neat::Genom::createMinimal(2, 1, mHistory);
   }

   neat::InnovationHistory mHistory;
};

BOOST_FIXTURE_TEST_CASE( TestSimpliest, NeuroNetTest ) 
{  
   neat::Genom a = createSampleGenom();

   {
      neat::NeuroNet n(a);

      BOOST_CHECK_EQUAL(0, n.activateLongTerm({0, 0})[0]);
   }
   
   a[0].weight = 0.5;
   a[1].weight = 0.5;

   {
      neat::NeuroNet n(a);

      BOOST_CHECK_EQUAL(10, n.activateLongTerm({10, 10})[0]);
   }

   a[0].weight = 0.5;
   a[1].weight = 0.25;

   {
      neat::NeuroNet n(a);

      BOOST_CHECK_EQUAL(7.5, n.activateLongTerm({10, 10})[0]);
   }
}

void printGenom(const neat::Genom& g)
{
   std::cout << "         (";

   for(auto& x : g)
   {
      if(x.enabled)
      {
         std::cout << x.srcNodeId << "->" << x.dstNodeId << " ";
      }
   }

   std::cout << ")" << std::endl;
}

BOOST_FIXTURE_TEST_CASE( TestOneHiddenNode, NeuroNetTest ) 
{  
   neat::Genom a = createSampleGenom();
   neat::mutateAddNode(a, mHistory);

   {
      a[0].weight = 0;
      a[1].weight = 0;
      a[2].weight = 0;
      a[3].weight = 0;

      neat::NeuroNet n(a);

      BOOST_CHECK_EQUAL(0, n.activateLongTerm({0, 0})[0]);
   }
   {
      a[0].weight = 0.5;
      a[1].weight = 0.5;
      a[2].weight = 0.5;
      a[3].weight = 0.5;

      std::cout << std::endl;
      printGenom(a);

      neat::NeuroNet n(a);

      BOOST_CHECK_EQUAL(5.5, n.activateLongTerm({10, 10})[0]);
   }
   {
      a[0].weight = 0.5;
      a[1].weight = 0.5;
      a[2].weight = 0.5;
      a[3].weight = 0.5;

      a += neat::Gene({3, 4, true, 0, 1.0});

      std::cout << std::endl;
      printGenom(a);

      neat::NeuroNet n(a);

      BOOST_CHECK_EQUAL(5.5, n.activateLongTerm({10, 10})[0]);
   }
}

BOOST_FIXTURE_TEST_CASE( TestTriangleNode, NeuroNetTest ) 
{
   //Innovation numbers are irrelivant in this test
   neat::Genom a = createSampleGenom();
   auto newNodeId1 = a.addNode();
   auto newNodeId2 = a.addNode();
   auto newNodeId3 = a.addNode();

   a[0].enabled = false;
   a[1].enabled = false;

   a += neat::Gene({a.getBiasNodeId(), newNodeId2, true, 0, 0.0});
   a += neat::Gene({a.getInputNodes()[0], newNodeId2, true, 0, 0.0});
   a += neat::Gene({a.getInputNodes()[0], newNodeId3, true, 0, 1.0});
   a += neat::Gene({a.getInputNodes()[1], newNodeId2, true, 0, 0.0});
   a += neat::Gene({a.getInputNodes()[1], newNodeId3, true, 0, 0.25});
   a += neat::Gene({newNodeId2, newNodeId1, true, 0, 0.5});
   a += neat::Gene({newNodeId1, newNodeId3, true, 0, 0.5});
   a += neat::Gene({newNodeId3, newNodeId2, true, 0, 0.5});
   a += neat::Gene({newNodeId1, a.getOutputNodes()[0], true, 0, 1.0});

   neat::NeuroNet n(a);
   BOOST_CHECK_EQUAL(0.5, n.activateLongTerm({10, 10})[0]);
}