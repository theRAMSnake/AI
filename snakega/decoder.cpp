#include "decoder.hpp"

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

   return (1 << 31) & index; 
}

bool isOutputNodeId(const neuroevolution::NodeId nodeId)
{
   return (nodeId >> 30) == 2;
}

neuroevolution::NodeId genHiddenNodeId(const std::size_t index)
{
   static_assert(sizeof(neuroevolution::NodeId) == 4);

   return (3 << 30) & index; 
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

class Neuron3DMap
{
public:
   Neuron3DMap(const neuroevolution::DomainGeometry& domainGeometry)
   {
      //Put all inputz with z=0.
      for(std::size_t i = 0; i < domainGeometry.inputs.size(); ++i)
      {
         auto& orig = domainGeometry.inputs[i];
         add(genInputNodeId(i), {double(orig.x) / domainGeometry.size.x, double(orig.y) / domainGeometry.size.y, 0});
      }

      //Put all outputs with z=1.
      for(std::size_t i = 0; i < domainGeometry.outputs.size(); ++i)
      {
         auto& orig = domainGeometry.outputs[i];
         add(genOutputNodeId(i), {double(orig.x) / domainGeometry.size.x, double(orig.y) / domainGeometry.size.y, 1.0});
      }
   }

   std::vector<neuroevolution::NodeId> findNClosestNeurons(const std::size_t amount, Point3D pos) const
   {
      //If slow - use smart algorithm
      std::vector<neuroevolution::NodeId> result;

      std::vector<double> distanceToPos(mPoints.size());

      for(std::size_t i = 0; i < mPoints.size(); ++i)
      {
         auto& pt = mPoints[i].second;
         distanceToPos[i] = sqrt((pt.x - pos.x) * (pt.x - pos.x) + (pt.y - pos.y) * (pt.y - pos.y) + (pt.z - pos.z) * (pt.z - pos.z));
      }

      std::sort(distanceToPos.begin(), distanceToPos.end());

      std::copy(distanceToPos.begin(), distanceToPos.begin() + std::min(distanceToPos.size(), amount), std::back_inserter(result));

      return result;
   }

   void add(const neuroevolution::NodeId id, const Point3D& pos)
   {
      mPoints.push_back({id, pos});
   }

private:
   std::vector<std::pair<neuroevolution::NodeId, Point3D>> mPoints;
};

GenomDecoder::GenomDecoder(const neuroevolution::DomainGeometry& domainGeometry, const Genom& src)
: mUniformWeights(-1.0, 1.0)
, mInputNodes(src.getNumInputs())
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

   mNeuron3DMap = std::make_shared<Neuron3DMap>(domainGeometry);
}

void GenomDecoder::processSpawnNeuron(const SpawnNeuronGene& s)
{
   auto id = genHiddenNodeId(mHiddenNodes.size());
   mHiddenNodes.push_back({id, s.af, s.bias});
   
   mRngConnections.seed(s.connectionsSeed);
   mRngWeigths.seed(s.weightsSeed);

   auto neighbours = mNeuron3DMap->findNClosestNeurons(s.numInputs + s.numOutputs, s.pos);

   for(std::size_t i = 0; i < s.numInputs && !neighbours.empty(); i++)
   {
      auto pos = neighbours.begin() + mRngConnections() % neighbours.size();

      if(isOutputNodeId(*pos))
      {
         continue;
      }

      mConnections.push_back({*pos, id, mUniformWeights(mRngWeigths)});

      neighbours.erase(pos);
   }

   for(std::size_t i = 0; i < s.numOutputs && !neighbours.empty(); i++)
   {
      auto pos = neighbours.begin() + mRngConnections() % neighbours.size();

      if(isInputNodeId(*pos))
      {
         continue;
      }

      mConnections.push_back({id, *pos, mUniformWeights(mRngWeigths)});

      neighbours.erase(pos);
   }

   mNeuron3DMap->add(id, s.pos);

   mCurrentBlock.push_back(s);
}

void GenomDecoder::run()
{
   for(auto& g : mSrc.mGenes)
   {
      if(std::holds_alternative<ConnectTerminalsGene>(g))
      {
         auto& c = std::get<ConnectTerminalsGene>(g);
         mConnections.push_back({genNodeId(c.A), genNodeId(c.B), c.weight});
      }
      else if(std::holds_alternative<SpawnNeuronGene>(g))
      {
         auto& s = std::get<SpawnNeuronGene>(g);

         processSpawnNeuron(s);
      }
      else if(std::holds_alternative<CopyWithOffsetGene>(g))
      {
         auto& c = std::get<CopyWithOffsetGene>(g);

         auto curBlock = mCurrentBlock;
         for(auto& s: curBlock)
         {
            auto shiftedS = s;
            shiftedS.pos += c.deltaPos;

            processSpawnNeuron(shiftedS);
         }
      }
      else if(std::holds_alternative<MirrorGene>(g))
      {
         auto& m = std::get<MirrorGene>(g);
         auto curBlock = mCurrentBlock;
         for(auto& s: curBlock)
         {
            auto mirrored = s;
            
            if(m.axis == Axis::X)
            {
               mirrored.pos.x = 1.0 - mirrored.pos.x;
            }
            else if(m.axis == Axis::Y)
            {
               mirrored.pos.y = 1.0 - mirrored.pos.y;
            }
            else
            {
               mirrored.pos.z = 1.0 - mirrored.pos.z;
            }

            processSpawnNeuron(mirrored);
         }
      }
      else if(std::holds_alternative<PushGene>(g))
      {
         mCurrentBlock.clear();
      }
      else
      {
         throw -1;
      }
   }
}

std::unique_ptr<neuroevolution::NeuroNet> GenomDecoder::decode(const neuroevolution::DomainGeometry& domainGeometry, const Genom& src)
{
   GenomDecoder decoder(domainGeometry, src);

   decoder.run();

   return std::make_unique<neuroevolution::NeuroNet>(decoder.mInputNodes, decoder.mOutputNodes, decoder.mHiddenNodes, decoder.mConnections);
}

}