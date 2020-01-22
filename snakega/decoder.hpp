#pragma once
#include <memory>
#include "genom.hpp"
#include "neuroevolution/neuro_net.hpp"

namespace snakega
{

class GenomDecoder
{
public:
   static std::unique_ptr<neuroevolution::NeuroNet> decode(const Genom& src);
};

}