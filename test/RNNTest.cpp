#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "neat/neuro_net.hpp"

class RNNTest
{
public:
   RNNTest()
   {
      neat::Config c;

      c.numInputs = 2;
      c.numOutputs = 1;
      c.population = 100;
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
       return neat::Genom::createMinimal(2, 1, mHistory, true);
   }

   neat::InnovationHistory mHistory;
};


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

BOOST_FIXTURE_TEST_CASE( TestRNN, RNNTest ) 
{  
   neat::Genom a = createSampleGenom();

   auto newNodeId1 = a.addNode();
   auto newNodeId2 = a.addNode();

   a[0].enabled = false;
   a[1].enabled = false;

   a += neat::Gene({a.getInputNodes()[0], newNodeId1, true, 0, 1.0});
   a += neat::Gene({a.getInputNodes()[1], newNodeId2, true, 0, 1.0});
   a += neat::Gene({newNodeId2, newNodeId1, true, 0, 1.0});
   a += neat::Gene({newNodeId1, a.getOutputNodes()[0], true, 0, 1.0});
   a += neat::Gene({a.getOutputNodes()[0], a.getOutputNodes()[0], true, 0, 1.0});

   neat::NeuroNet n(a);
   BOOST_CHECK_EQUAL(0.92414181997875655, neat::activate(n, {0, 0})[0]);
   BOOST_CHECK_EQUAL(1.8482836399575131, neat::activate(n, {0, 0})[0]);
   BOOST_CHECK_EQUAL(2.7724254599362697, neat::activate(n, {0, 0})[0]);
}