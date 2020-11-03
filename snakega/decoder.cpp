#include "decoder.hpp"
#include "logger//Logger.hpp"

namespace snakega
{

GenomDecoder::GenomDecoder(const Genom& src)
: mInputNodes(src.getNumInputs())
, mOutputNodes(src.getNumOutputs())
, mSrc(src)
{
   
}

void GenomDecoder::run()
{
   throw - 1;
}

std::unique_ptr<neuroevolution::NeuroNet> GenomDecoder::decode(const Genom& src)
{
   GenomDecoder decoder(src);

   decoder.run();

   return std::make_unique<neuroevolution::NeuroNet>(decoder.mInputNodes, decoder.mOutputNodes, decoder.mHiddenNodes, decoder.mConnections);
}

}