#pragma once
#include <vector>
#include <memory>
#include "neuroevolution/neuro_net.hpp"
#include "neuroevolution/IPlayground.hpp"
#include "genom.hpp"

namespace neat
{

class Substrate3D
{
public:
   Substrate3D(const neuroevolution::DomainGeometry& domainGeometry);

   std::unique_ptr<neuroevolution::NeuroNet> apply(const v2::Genom& src) const;

private:
   const std::vector<neuroevolution::Point2D>& getLayer(const std::size_t n) const;

   neuroevolution::DomainGeometry mGeometry;

   std::vector<neuroevolution::NodeId> mInputNodes; 
   std::vector<neuroevolution::NodeId> mBiasNodes;
   std::vector<neuroevolution::NodeId> mOutputNodes;

   std::vector<neuroevolution::Point2D> mHiddenPlaneNodes;
};

}