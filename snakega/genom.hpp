#pragma once
#include <vector>
#include <variant>
#include "neuroevolution/IPlayground.hpp"
#include "neuroevolution/activation.hpp"

namespace snakega
{

struct MutationConfig
{

};

struct Point3D
{
   double x;
   double y;
   double z;

   void operator += (const Point3D& other)
   {
      x += other.x;
      y += other.y;
      z += other.z;
   }
};

using TerminalId = unsigned int;
using NeuronId = TerminalId;

enum TerminalType
{
   Input,
   Output,
   Neuron
};

struct Terminal
{
   TerminalType type;
   TerminalId id;
};

//Explicitly connects two terminals.
//If neuron with specific id is not found gene has no effect
struct ConnectTerminalsGene
{
   Terminal A;
   Terminal B;

   //parameters:
   double weight;
};

//Spawns neuron at specific pos and connect it to other neurons.
//Using seeds, other neurons and connection weights are determined. Closer neurons has move chances to be selected.
struct SpawnNeuronGene
{
   Point3D pos;
   unsigned int numInputs;
   unsigned int numOutputs;
   unsigned int connectionsSeed; //To keep determinisic

   //parameters:
   ActivationFunctionType af;
   double bias;
   unsigned int weightsSeed; //To keep determinisic
};

//Copies current block, by applying position offset 
struct CopyWithOffsetGene
{
   Point3D deltaPos;
};

enum class Axis
{
   X,
   Y,
   Z
};

//Mirrors current block by axis
struct MirrorGene
{
   Axis axis;
};

//Pushes current block to the structure
struct PushGene
{

};

using Gene = std::variant<ConnectTerminalsGene, SpawnNeuronGene, CopyWithOffsetGene, MirrorGene, PushGene>;

//Note: 3D area's dimensions is 0..1
class Genom
{
public:
   Genom(const std::size_t numInputs, const std::size_t numOutputs);

   void operator= (const Genom& other);

   static Genom createHalfConnected(const std::size_t numInputs, const std::size_t numOutputs);

   void mutateStructure(const MutationConfig& mutationConfig);
   void mutateParameters(const MutationConfig& mutationConfig);

private:
   void mutateAddGene(const MutationConfig& mutationConfig);
   void mutateSwapGenes();
   void mutateRemoveGene();
   void mutateChangeGene();

   Terminal genRandomTerminal() const;
   Point3D genPos() const;
   Point3D genOffset() const;
   Point3D genSmallPosOffset() const;

   void updateNumNeurons();

   std::vector<Gene> mGenes;

   unsigned int mNumNeurons = 0;
   const std::size_t mNumInputs;
   const std::size_t mNumOutputs;
};

}