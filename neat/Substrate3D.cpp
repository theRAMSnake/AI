#include "Substrate3D.hpp"

namespace neat
{

inline neuroevolution::NodeId genNodeId(const unsigned int x, const unsigned int y, const unsigned int z)
{
   static_assert(sizeof(neuroevolution::NodeId) > 3);

   return z << 16 | y << 8 | x; 
}

Substrate3D::Substrate3D(const DomainGeometry& domainGeometry)
: mGeometry(domainGeometry)
{
   if(domainGeometry.size.z < 3 || domainGeometry.size.z > 254 || domainGeometry.size.x > 254 || domainGeometry.size.y > 254)
   {
      //Should be at least 1 hidden layer
      //Should be less than 254 x/y/z due to node id to coordinate convention
      throw -1;
   }

   for(unsigned int i = 0; i < domainGeometry.size.z - 2; ++i)
   {
      //One bias per hidden layer at [0, 0]
      mBiasNodes.push_back(genNodeId(0, 0, i + 1));
   }

   for(auto& n : domainGeometry.inputs)
   {
      mInputNodes.push_back(genNodeId(n.x, n.y, 0));
   }

   for(auto& n : domainGeometry.outputs)
   {
      mOutputNodes.push_back(genNodeId(n.x, n.y, domainGeometry.size.z - 1));
   }
}

std::unique_ptr<neuroevolution::NeuroNet> Substrate3D::apply(const v2::Genom& src) const
{
   const double THRESHOLD = 0.2;

   auto srcAnn = v2::createAnn(src);

   //Restriction: only allow connections on the same or neighbour layers.
   std::vector<neuroevolution::NeuroNet::ConnectionDef> connections;

   for(unsigned int srcLayer = 0; srcLayer != mGeometry.size.z; ++srcLayer)
   {
      if(srcLayer == 0) //Special case: inputs to hidden layer 1 connections
      {
         for(auto& n : mGeometry.inputs)
         {
            for(auto& pt : mHiddenLayerNodes)
            {
               //Skip bias
               auto w = neuroevolution::activate(*srcAnn, {n.x, n.y, 0, pt.x, pt.y, srcLayer + 1})[0];
            }
         }
      }
   }     

   std::vector<std::pair<neuroevolution::NodeId, ActivationFunctionType>> hiddenNodes; // = fromConnections
   return std::make_unique<neuroevolution::NeuroNet>(mInputNodes, mBiasNodes, mOutputNodes, hiddenNodes, connections);
}

}