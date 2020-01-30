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
struct ConnectTerminalsGene
{
   Terminal A;
   Terminal B;

   //parameters:
   double weight;
};

struct SpawnNeuronGene
{
   Point3D pos;
   unsigned int id;

   //parameters:
   ActivationFunctionType af;
   double bias;
};

//Note: 3D area's dimensions is 0..1
//Direct I/O connections are prohibited
class Genom
{
public:
   friend class GenomDecoder;
   Genom(const std::vector<Point3D>& inputs, const std::vector<Point3D>& outputs);

   void operator= (const Genom& other);

   static Genom createHalfConnected(const std::vector<Point3D>& inputs, const std::vector<Point3D>& outputs);

   void mutateStructure(const MutationConfig& mutationConfig);
   void mutateParameters(const MutationConfig& mutationConfig);

   unsigned int getNumNeurons() const;
   unsigned int getComplexity() const;
   unsigned int getNumInputs() const;
   unsigned int getNumOutputs() const;

   static Genom loadState(std::ifstream& s, const std::vector<Point3D>& inputs, const std::vector<Point3D>& outputs);
   void saveState(std::ofstream& s) const;

protected:
   std::vector<ConnectTerminalsGene> mConnections;
   std::vector<SpawnNeuronGene> mNeurons;

private:
   Terminal genRandomTerminal(const Point3D& srcPos, const bool isSrc) const;
   Point3D genPos() const;
   Point3D getPos(const Terminal& terminal) const;
   unsigned int genNeuronId();

   const std::size_t mNumInputs;
   const std::size_t mNumOutputs;
   const std::vector<Point3D>& mInputs;
   const std::vector<Point3D>& mOutputs;
};

}