#include "decoder.hpp"
#include <boost/random.hpp>

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
   Neuron3DMap(const neuroevolution::DomainGeometry& domainGeometry);//Put I/O

   std::vector<neuroevolution::NodeId> findNClosestNeurons(const std::size_t amount, Point3D pos) const;

   void add(const neuroevolution::NodeId id, const Point3D& pos);
};

std::unique_ptr<neuroevolution::NeuroNet> GenomDecoder::decode(const neuroevolution::DomainGeometry& domainGeometry, const Genom& src)
{
   boost::random::mt19937 rngConnections;
   boost::random::mt19937 rngWeigths;
   boost::uniform_real<> uniformWeights(-1.0, 1.0);

   std::vector<neuroevolution::NodeId> inputNodes(src.getNumInputs()); //Always same can be optimized
   std::vector<neuroevolution::NodeId> outputNodes(src.getNumOutputs()); //Always same can be optimized
   std::vector<neuroevolution::NeuroNet::HiddenNodeDef> hiddenNodes;
   std::vector<neuroevolution::NeuroNet::ConnectionDef> connections;

   for(std::size_t i = 0; i < src.getNumInputs(); ++i)
   {
      inputNodes[i] = genInputNodeId(i);
   }

   for(std::size_t i = 0; i < src.getNumOutputs(); ++i)
   {
      outputNodes[i] = genOutputNodeId(i);
   }

   Neuron3DMap map(domainGeometry);

   for(auto& g : src.mGenes)
   {
      if(std::holds_alternative<ConnectTerminalsGene>(g))
      {
         auto& c = std::get<ConnectTerminalsGene>(g);
         connections.push_back({genNodeId(c.A), genNodeId(c.B), c.weight});
      }
      else if(std::holds_alternative<SpawnNeuronGene>(g))
      {
         auto& s = std::get<SpawnNeuronGene>(g);
         auto id = genHiddenNodeId(hiddenNodes.size());
         hiddenNodes.push_back({id, s.af, s.bias});
         
         rngConnections.seed(s.connectionsSeed);
         rngWeigths.seed(s.weightsSeed);

         auto neighbours = map.findNClosestNeurons(s.numInputs + s.numOutputs, s.pos);

         for(std::size_t i = 0; i < s.numInputs && !neighbours.empty(); i++)
         {
            auto pos = neighbours.begin() + rngConnections() % neighbours.size();

            if(isOutputNodeId(*pos))
            {
               continue;
            }

            connections.push_back({*pos, id, uniformWeights(rngWeigths)});

            neighbours.erase(pos);
         }

         for(std::size_t i = 0; i < s.numOutputs && !neighbours.empty(); i++)
         {
            auto pos = neighbours.begin() + rngConnections() % neighbours.size();

            if(isInputNodeId(*pos))
            {
               continue;
            }

            connections.push_back({id, *pos, uniformWeights(rngWeigths)});

            neighbours.erase(pos);
         }

         map.add(id, s.pos);
      }
      else if(std::holds_alternative<CopyWithOffsetGene>(g))
      {

      }
      else if(std::holds_alternative<MirrorGene>(g))
      {

      }
      else if(std::holds_alternative<PushGene>(g))
      {

      }
      else
      {
         throw -1;
      }
   }

   return std::make_unique<neuroevolution::NeuroNet>(inputNodes, outputNodes, hiddenNodes, connections);
}

}