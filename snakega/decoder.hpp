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
   static std::unique_ptr<neuroevolution::NeuroNet> decode(const neuroevolution::DomainGeometry& domainGeometry, const Genom& src);

private:
   GenomDecoder(const neuroevolution::DomainGeometry& domainGeometry, const Genom& src);

   void run();
   void processSpawnNeuron(const SpawnNeuronGene& s);

   boost::random::mt19937 mRngConnections;
   boost::random::mt19937 mRngWeigths;
   boost::uniform_real<> mUniformWeights;

   std::vector<neuroevolution::NodeId> mInputNodes; //Always same can be optimized
   std::vector<neuroevolution::NodeId> mOutputNodes; //Always same can be optimized
   std::vector<neuroevolution::NeuroNet::HiddenNodeDef> mHiddenNodes;
   std::vector<neuroevolution::NeuroNet::ConnectionDef> mConnections;
   std::shared_ptr<Neuron3DMap> mNeuron3DMap;
   const Genom& mSrc;

   std::vector<SpawnNeuronGene> mCurrentBlock;
};

}