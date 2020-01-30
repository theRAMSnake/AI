#include "genom.hpp"
#include "neuroevolution/rng.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include "logger/Logger.hpp"

namespace snakega
{

Genom::Genom(const std::vector<Point3D>& inputs, const std::vector<Point3D>& outputs)
: mNumInputs(inputs.size())
, mNumOutputs(outputs.size())
, mInputs(inputs)
, mOutputs(outputs)
{

}

Genom Genom::createHalfConnected(const std::vector<Point3D>& inputs, const std::vector<Point3D>& outputs)
{
   //Create one fully connected hidden layer
   Genom result(inputs, outputs);

   for(std::size_t i = 0; i < result.mNumInputs; ++i)
   {
      result.mNeurons.push_back({
         {inputs[i].x, inputs[i].y, 0.5}, 
         result.genNeuronId(),
         static_cast<ActivationFunctionType>(Rng::genChoise(NUM_ACTIVATION_FUNCTION_TYPES)), 
         0.0}
         );
   }

   for(auto &n : result.mNeurons)
   {
      for(std::size_t i = 0; i < result.mNumInputs; ++i)
      {
         if(Rng::genProbability(0.5))
         {
            result.mConnections.push_back(ConnectTerminalsGene{
               {TerminalType::Input, static_cast<TerminalId>(i)}, 
               {TerminalType::Neuron, n.id},
               Rng::genWeight()
               });
         }
      }
      for(std::size_t j = 0; j < result.mNumOutputs; ++j)
      {
         if(Rng::genProbability(0.5))
         {
            result.mConnections.push_back(ConnectTerminalsGene{
               {TerminalType::Neuron, n.id}, 
               {TerminalType::Output, static_cast<TerminalId>(j)},
               Rng::genWeight()
               });
         }
      }
   }

   return result;
}

void Genom::mutateStructure(const MutationConfig& mutationConfig)
{
   //Each neuron can raise connections each generation, depending on the distance to target
   for(auto& n : mNeurons)
   {
      if(Rng::genProbability(0.02))
      {
         bool isSrc = Rng::genProbability(0.5);
         if(isSrc)
         {
            mConnections.push_back(ConnectTerminalsGene{
               {TerminalType::Neuron, n.id}, 
               genRandomTerminal(n.pos, false),
               Rng::genWeight()
               });
         }
         else
         {
            mConnections.push_back(ConnectTerminalsGene{
               genRandomTerminal(n.pos, true),
               {TerminalType::Neuron, n.id}, 
               Rng::genWeight()
               });
         }
      }
   }

   //Drop amount of connections based on number of neurons
   if(!mConnections.empty())
   {
      for(std::size_t i = 0; i < mNeurons.size(); ++i)
      {
         if(Rng::genProbability(0.01))
         {
            mConnections.erase(mConnections.begin() + Rng::genChoise(mConnections.size()));
         }
      }
   }

   //Each generation a new neurons(depending on the size) are born at random position, connected to some others
   for(std::size_t i = 0; i < mNeurons.size() * 0.02 + 1; ++i)
   {
      if(Rng::genProbability(0.5))
      {
         auto id = genNeuronId();
         auto pos = genPos();
         mNeurons.push_back({
            pos, 
            id,
            static_cast<ActivationFunctionType>(Rng::genChoise(NUM_ACTIVATION_FUNCTION_TYPES)), 
            0.0}
            );

         for(unsigned int x = 0; x < Rng::genChoise(5); ++x)
         {
            mConnections.push_back(ConnectTerminalsGene{
               {TerminalType::Neuron, id}, 
               genRandomTerminal(pos, false),
               Rng::genWeight()
               });
         }

         for(unsigned int y = 0; y < Rng::genChoise(5); ++y)
         {
            mConnections.push_back(ConnectTerminalsGene{
               genRandomTerminal(pos, true),
               {TerminalType::Neuron, id}, 
               Rng::genWeight()
               });
         }
      }
   }

   //Each generation set of neurons are destroyed(depending on the size)
   for(std::size_t i = 0; i < mNeurons.size() * 0.02; ++i)
   {
      if(Rng::genProbability(0.25))
      {
         auto pos = mNeurons.begin() + Rng::genChoise(mNeurons.size());

         mConnections.erase(std::remove_if(mConnections.begin(), mConnections.end(), [&](auto x){
            return (x.A.id == pos->id && x.A.type == TerminalType::Neuron) ||
            (x.B.id == pos->id && x.B.type == TerminalType::Neuron);}),
            mConnections.end());

         mNeurons.erase(pos);
      }
   }
}

void Genom::mutateParameters(const MutationConfig& mutationConfig)
{
   for(auto& c : mConnections)
   {
      if(Rng::genProbability(0.8))
      {
         c.weight += Rng::genPerturbation();
      }
      else
      {
         c.weight = Rng::genWeight();
      }
   }
}

Terminal Genom::genRandomTerminal(const Point3D& srcPos, const bool isSrc) const
{
   Terminal result;

   while(true)
   {
      if(isSrc && Rng::genProbability(mNumInputs / (double)(mNumInputs + mNeurons.size())))
      {
         result.type = TerminalType::Input;
         result.id = Rng::genChoise(mNumInputs);
      }
      else if(!isSrc && Rng::genProbability(mNumOutputs / (double)(mNumOutputs + mNeurons.size())))
      {
         result.type = TerminalType::Output;
         result.id = Rng::genChoise(mNumOutputs);
      }
      else
      {
         result.type = TerminalType::Neuron;
         result.id = Rng::genChoise(mNeurons.size());
      }

      auto otherPos = getPos(result);
      auto distance = sqrt((srcPos.x - otherPos.x) * (srcPos.x - otherPos.x) + 
         (srcPos.y - otherPos.y) * (srcPos.y - otherPos.y) + 
         (srcPos.z - otherPos.z) * (srcPos.z - otherPos.z));

      if(distance == 0.0 || Rng::genProbability(1.0 / std::exp(5 * distance)))
      {
         break;
      }
   }

   return result;
}

Point3D Genom::getPos(const Terminal& terminal) const
{
   switch(terminal.type)
   {
      case TerminalType::Input:
         return mInputs[terminal.id];

      case TerminalType::Output:
         return mOutputs[terminal.id];

      case TerminalType::Neuron:
         return std::find_if(mNeurons.begin(), mNeurons.end(), [&](auto x){return x.id == terminal.id;})->pos;
   }

   throw -1;
}

Point3D Genom::genPos() const
{
   return {Rng::genReal(), Rng::genReal(), Rng::genReal()};
}

void Genom::operator= (const Genom& other)
{
   if(other.mNumInputs != mNumInputs || other.mNumOutputs != mNumOutputs)
   {
      throw -1;
   }
   mNeurons = other.mNeurons;
   mConnections = other.mConnections;
}

unsigned int Genom::getNumNeurons() const
{
   return mNeurons.size();
}

unsigned int Genom::getComplexity() const
{
   return mConnections.size();
}

unsigned int Genom::getNumInputs() const
{
   return mNumInputs;
}

unsigned int Genom::getNumOutputs() const
{
   return mNumOutputs;
}

Genom Genom::loadState(std::ifstream& s, const std::vector<Point3D>& inputs, const std::vector<Point3D>& outputs)
{
   Genom g(inputs, outputs);

   std::size_t size = 0;
   s.read((char*)&size, sizeof(std::size_t));

   g.mNeurons.resize(size);
   s.read(reinterpret_cast<char*>(&g.mNeurons[0]), size * sizeof(SpawnNeuronGene));

   s.read((char*)&size, sizeof(std::size_t));

   g.mConnections.resize(size);
   s.read(reinterpret_cast<char*>(&g.mConnections[0]), size * sizeof(ConnectTerminalsGene));

   return g;
}

void Genom::saveState(std::ofstream& s) const
{
   auto sz = mNeurons.size();
   s.write((char*)&sz, sizeof(std::size_t));  
   s.write((char*)&mNeurons[0], sizeof(SpawnNeuronGene) * mNeurons.size());  

   sz = mConnections.size();
   s.write((char*)&sz, sizeof(std::size_t));  
   s.write((char*)&mConnections[0], sizeof(ConnectTerminalsGene) * mConnections.size());  
}

unsigned int Genom::genNeuronId()
{
   if(mNeurons.empty())
   {
      return 0;
   }
   else
   {
      return mNeurons.back().id + 1;
   }
}

}