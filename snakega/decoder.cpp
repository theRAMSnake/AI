#include "decoder.hpp"
#include "logger//Logger.hpp"

namespace snakega
{

neuroevolution::NodeId genInputNodeId(const std::size_t index)
{
   static_assert(sizeof(neuroevolution::NodeId) == 4);

   return index; 
}

bool isInputNodeId(const neuroevolution::NodeId nodeId)
{
   return (nodeId >> 31) == 0;
}

neuroevolution::NodeId genOutputNodeId(const std::size_t index)
{
   static_assert(sizeof(neuroevolution::NodeId) == 4);

   return (neuroevolution::NodeId(1) << 31) | index; 
}

bool isOutputNodeId(const neuroevolution::NodeId nodeId)
{
   return (nodeId >> 30) == 2;
}

neuroevolution::NodeId genHiddenNodeId(const std::size_t index)
{
   static_assert(sizeof(neuroevolution::NodeId) == 4);

   return (neuroevolution::NodeId(3) << 30) | index; 
}

neuroevolution::NodeId genNodeId(const Terminal& t)
{
   static_assert(sizeof(neuroevolution::NodeId) == 4);

   switch(t.type)
   {
   case TerminalType::Input:
      return genInputNodeId(t.id);

   case TerminalType::Output:
      return genOutputNodeId(t.id);

   case TerminalType::Neuron:
      return genHiddenNodeId(t.id);
   }

   return 0;
}

GenomDecoder::GenomDecoder(const neuroevolution::DomainGeometry& domainGeometry, const Genom& src)
: mInputNodes(src.getNumInputs())
, mOutputNodes(src.getNumOutputs())
, mSrc(src)
{
   for(std::size_t i = 0; i < src.getNumInputs(); ++i)
   {
      mInputNodes[i] = genInputNodeId(i);
   }

   for(std::size_t i = 0; i < src.getNumOutputs(); ++i)
   {
      mOutputNodes[i] = genOutputNodeId(i);
   }
}

void GenomDecoder::run()
{
   for(auto& g : mSrc.mConnections)
   {
      mConnections.push_back({genNodeId(g.A), genNodeId(g.B), g.weight});
   }

   for(auto& g : mSrc.mNeurons)
   {
      auto id = genHiddenNodeId(mHiddenNodes.size());
      mHiddenNodes.push_back({id, g.af, g.bias});
   }
}

std::unique_ptr<neuroevolution::NeuroNet> GenomDecoder::decode(const neuroevolution::DomainGeometry& domainGeometry, const Genom& src)
{
   GenomDecoder decoder(domainGeometry, src);

   decoder.run();

   return std::make_unique<neuroevolution::NeuroNet>(decoder.mInputNodes, decoder.mOutputNodes, decoder.mHiddenNodes, decoder.mConnections);
}

}