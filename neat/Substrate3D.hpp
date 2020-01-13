#pragma once
#include <vector>
#include <memory>
#include "neuroevolution/neuro_net.hpp"
#include "genom.hpp"

namespace neat
{

struct Point3D
{
   unsigned int x;
   unsigned int y;
   unsigned int z;
};

struct Point2D
{
   unsigned int x;
   unsigned int y;
};

struct DomainGeometry
{
   Point3D size;
   std::vector<Point2D> inputs;
   std::vector<Point2D> outputs;
};

class Substrate3D
{
public:
   Substrate3D(const DomainGeometry& domainGeometry);

   std::unique_ptr<neuroevolution::NeuroNet> apply(const v2::Genom& src) const;

private:
   DomainGeometry mGeometry;

   std::vector<neuroevolution::NodeId> mInputNodes; 
   std::vector<neuroevolution::NodeId> mBiasNodes;
   std::vector<neuroevolution::NodeId> mOutputNodes;
};

}