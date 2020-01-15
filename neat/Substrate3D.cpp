#include "Substrate3D.hpp"
#include <set>

namespace neat
{

inline neuroevolution::NodeId genNodeId(const unsigned int x, const unsigned int y, const unsigned int z)
{
   static_assert(sizeof(neuroevolution::NodeId) > 3);

   return z << 16 | y << 8 | x; 
}

Substrate3D::Substrate3D(const neuroevolution::DomainGeometry& domainGeometry)
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

   for(unsigned int i = 0; i < domainGeometry.size.x; ++i)
   {
      for(unsigned int j = 0; j < domainGeometry.size.y; ++j)
      {
         mHiddenPlaneNodes.push_back({i, j});
      }
   }
}

void genConnections(
   const std::vector<neuroevolution::Point2D>& src, 
   unsigned int srcLayer, 
   const std::vector<neuroevolution::Point2D>& dst, 
   unsigned int dstLayer,
   const bool destinationIsOutput,
   neuroevolution::NeuroNet& cpnn,
   std::vector<neuroevolution::NeuroNet::ConnectionDef>& out,
   std::set<neuroevolution::NodeId>& hiddenNodesSet
   )
{
   const double THRESHOLD = 0.2;

   for(auto& s : src)
   {
      for(auto& d : dst)
      {
         if(s.x == 0 && s.y == 0) //Skip bias
         {
            continue;
         }

         auto w = neuroevolution::activate(cpnn, {double(s.x), double(s.y), double(srcLayer), double(d.x), double(d.y), double(dstLayer)})[0];
         if(std::abs(w) > THRESHOLD)
         {
            out.push_back({genNodeId(s.x, s.y, srcLayer), genNodeId(d.x, d.y, dstLayer), w /*Should we normalize*/});

            if(!destinationIsOutput)
            {
               hiddenNodesSet.insert(genNodeId(d.x, d.y, dstLayer));
            }
         }
      }
   }
}

std::unique_ptr<neuroevolution::NeuroNet> Substrate3D::apply(const v2::Genom& src) const
{
   auto srcAnn = v2::createAnn(src);

   //Restriction: only allow connections on the same or neighbour layers.
   std::vector<neuroevolution::NeuroNet::ConnectionDef> connections;
   std::set<neuroevolution::NodeId> hiddenNodesSet;

   //1. Connect inputs to layer1
   genConnections(mGeometry.inputs, 0, mHiddenPlaneNodes, 1, false, *srcAnn, connections, hiddenNodesSet);

   //2. Connect each layer n to layer n+1 and itself
   for(unsigned int srcLayer = 1; srcLayer != mGeometry.size.z - 1; ++srcLayer)
   {
      genConnections(mHiddenPlaneNodes, srcLayer, mHiddenPlaneNodes, srcLayer, false, *srcAnn, connections, hiddenNodesSet);
      if(srcLayer != mGeometry.size.z - 2) //Last layer will connect ot outputs
      {
         genConnections(mHiddenPlaneNodes, srcLayer, mHiddenPlaneNodes, srcLayer + 1, false, *srcAnn, connections, hiddenNodesSet);
      }
   }     

   //3. Connect last layer to outputs
   genConnections(mHiddenPlaneNodes, mGeometry.size.z - 2, mGeometry.outputs, mGeometry.size.z - 1, true, *srcAnn, connections, hiddenNodesSet);

   std::vector<std::pair<neuroevolution::NodeId, ActivationFunctionType>> hiddenNodes;
   for(auto& n : hiddenNodesSet)
   {
      hiddenNodes.push_back({n, ActivationFunctionType::SIGMOID});
   }

   return std::make_unique<neuroevolution::NeuroNet>(mInputNodes, mBiasNodes, mOutputNodes, hiddenNodes, connections);
}

}