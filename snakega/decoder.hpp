#pragma once
#include <memory>
#include "genom.hpp"
#include <boost/random.hpp>
#include "neuroevolution/neuro_net.hpp"

namespace snakega
{

class Neuron3DMap;
class GenomDecoder
{
public:
   static std::unique_ptr<neuroevolution::NeuroNet> decode(const Genom& src);

private:
   GenomDecoder(const Genom& src);

   neuroevolution::NodeId makeNodeId(const Genom::GlobalNodeId src) const;

   void run();

   std::vector<neuroevolution::NodeId> mInputNodes; //Always same can be optimized
   std::vector<neuroevolution::NodeId> mOutputNodes; //Always same can be optimized
   std::vector<neuroevolution::NeuroNet::HiddenNodeDef> mHiddenNodes;
   std::vector<neuroevolution::NeuroNet::ConnectionDef> mConnections;
   const Genom& mSrc;
};

}