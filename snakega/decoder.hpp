#pragma once
#include <memory>

namespace snakega
{

class GenomDecoder
{
public:
   static std::unique_ptr<neuroevolution::NeuroNet> decode(const Genom& src);
};

}