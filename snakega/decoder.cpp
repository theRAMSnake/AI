#include "decoder.hpp"
#include "logger//Logger.hpp"

namespace snakega
{

GenomDecoder::GenomDecoder(const Genom& src)
: mInputNodes(src.getNumInputs())
, mOutputNodes(src.getNumOutputs())
, mSrc(src)
{
   for(std::size_t i = 0; i < src.getNumInputs(); ++i)
   {
      mInputNodes[i] = makeNodeId(Genom::GlobalNodeId{Genom::GlobalNodeType::Input, 0, i});
   }

   for(std::size_t i = 0; i < src.getNumOutputs(); ++i)
   {
      mOutputNodes[i] = makeNodeId(Genom::GlobalNodeId{Genom::GlobalNodeType::Output, 0, i});
   }
}

neuroevolution::NodeId GenomDecoder::makeNodeId(const Genom::GlobalNodeId src) const
{
   return static_cast<neuroevolution::NodeId>(src.localId) | (static_cast<neuroevolution::NodeId>(src.blockId) << 16) |
      (static_cast<neuroevolution::NodeId>(src.type) << 30);
}

void GenomDecoder::run()
{
   for(auto iter = mSrc.beginNodes(); iter != mSrc.endNodes(); ++iter)
   {
      mHiddenNodes.push_back({makeNodeId(iter->id), iter->acType, 0.0});
   }

   for(auto iter = mSrc.beginConnections(); iter != mSrc.endConnections(); ++iter)
   {
      mConnections.push_back({makeNodeId(iter->srcNodeId), makeNodeId(iter->dstNodeId), iter->weight});
   }
}

std::unique_ptr<neuroevolution::NeuroNet2> GenomDecoder::decode(const Genom& src)
{
   GenomDecoder decoder(src);

   decoder.run();

   return std::make_unique<neuroevolution::NeuroNet2>(decoder.mInputNodes, decoder.mOutputNodes, decoder.mHiddenNodes, decoder.mConnections);
}

}