#include "genom.hpp"
#include "neuroevolution/rng.hpp"

namespace snakega
{

Genom::Genom(const std::size_t numInputs, const std::size_t numOutputs)
: mNumInputs(numInputs)
, mNumOutputs(numOutputs)
{

}

Genom Genom::createHalfConnected(const std::size_t numInputs, const std::size_t numOutputs)
{
   Genom result(numInputs, numOutputs);

   for(std::size_t i = 0; i < numInputs; ++i)
   {
      for(std::size_t j = 0; j < numOutputs; ++j)
      {
         if(Rng::genProbability(0.5))
         {
            result.mGenes.push_back(ConnectTerminalsGene{
               {TerminalType::Input, static_cast<TerminalId>(i)}, 
               {TerminalType::Output, static_cast<TerminalId>(j)},
               Rng::genWeight()
               });
         }      
      }
   }

   result.mGenes.push_back(PushGene{});

   return result;
}

void Genom::mutateStructure(const MutationConfig& mutationConfig)
{
   enum StructureMutationType
   {
      Add,
      Remove,
      Swap,
      Change
   };

   StructureMutationType choise = static_cast<StructureMutationType>(Rng::genChoise(4));

   switch(choise)
   {
      case Add:
         mutateAddGene(mutationConfig);
         break;
      case Remove:
         mutateRemoveGene();
         break;
      case Swap:
         mutateSwapGenes();
         break;
      case Change:
         mutateChangeGene();
         break;
   }

   updateNumNeurons();
}

void Genom::mutateParameters(const MutationConfig& mutationConfig)
{
   for(auto& g : mGenes)
   {
      if(std::holds_alternative<ConnectTerminalsGene>(g))
      {
         if(Rng::genProbability(0.8))
         {
            std::get<ConnectTerminalsGene>(g).weight += Rng::genPerturbation();
         }
         else
         {
            std::get<ConnectTerminalsGene>(g).weight = Rng::genWeight();
         }
      }
      else if(std::holds_alternative<SpawnNeuronGene>(g))
      {
         auto& spawnGene = std::get<SpawnNeuronGene>(g);

         switch(Rng::genChoise(3))
         {
            case 0:
               spawnGene.af = static_cast<ActivationFunctionType>(Rng::genChoise(NUM_ACTIVATION_FUNCTION_TYPES));
               break;

            case 1:
               spawnGene.bias += Rng::genPerturbation();
               break;

            case 2:
               spawnGene.weightsSeed = Rng::gen32();
               break;

            default:
               throw -1;
         }
      }
   }
}

void Genom::mutateAddGene(const MutationConfig& mutationConfig)
{
   //Configurate later
   if(mNumNeurons > 0 && Rng::genProbability(0.5))
   {
      mGenes.push_back(ConnectTerminalsGene{
         genRandomTerminal(), 
         genRandomTerminal(),
         Rng::genWeight()
         });
   }
   if(Rng::genProbability(0.2))
   {
      mGenes.push_back(SpawnNeuronGene{
         genPos(), 
         Rng::genChoise(5) + 1, //1-5 inputs
         Rng::genChoise(3) + 1, //1-3 outputs
         Rng::gen32(),
         static_cast<ActivationFunctionType>(Rng::genChoise(NUM_ACTIVATION_FUNCTION_TYPES)),
         0.0,
         Rng::gen32()
         });
   }
   if(Rng::genProbability(0.05))
   {
      mGenes.push_back(CopyWithOffsetGene{
         genOffset()
         });
   }
   if(Rng::genProbability(0.05))
   {
      mGenes.push_back(MirrorGene{
         static_cast<Axis>(Rng::genChoise(3))
         });
   }
   if(Rng::genProbability(0.1))
   {
      mGenes.push_back(PushGene{
         });
   }
}

void Genom::mutateRemoveGene()
{
   if(!mGenes.empty())
   {
      mGenes.erase(mGenes.begin() + Rng::genChoise(mGenes.size()));
   }
}

void Genom::mutateSwapGenes()
{
   if(!mGenes.empty())
   {
      std::swap(*(mGenes.begin() + Rng::genChoise(mGenes.size())), *(mGenes.begin() + Rng::genChoise(mGenes.size())));
   }
}

void Genom::mutateChangeGene()
{
   if(!mGenes.empty())
   {
      auto& gene = *(mGenes.begin() + Rng::genChoise(mGenes.size()));

      if(std::holds_alternative<SpawnNeuronGene>(gene))
      {
         auto& spawnGene = std::get<SpawnNeuronGene>(gene);

         //move pos slightly
         spawnGene.pos += genSmallPosOffset();

         //change seed
         spawnGene.connectionsSeed += Rng::gen32();

         //increase/decrease num connections by 1
         spawnGene.numInputs += int(Rng::genChoise(3)) - 1;
         spawnGene.numOutputs += int(Rng::genChoise(3)) - 1;
      }
      else if(std::holds_alternative<CopyWithOffsetGene>(gene))
      {
         std::get<CopyWithOffsetGene>(gene).deltaPos += genSmallPosOffset();
      }
   }
}

Terminal Genom::genRandomTerminal() const
{
   Terminal result;

   if(Rng::genProbability(0.05))
   {
      result.type = TerminalType::Input;
      result.id = Rng::genChoise(mNumInputs);
   }
   else if(Rng::genProbability(0.05))
   {
      result.type = TerminalType::Output;
      result.id = Rng::genChoise(mNumOutputs);
   }
   else
   {
      result.type = TerminalType::Neuron;
      result.id = Rng::genChoise(mNumNeurons);
   }

   return result;
}

Point3D Genom::genPos() const
{
   return {Rng::genReal(), Rng::genReal(), Rng::genReal()};
}

Point3D Genom::genOffset() const
{
   return {Rng::genReal() * 0.1 - 0.05, Rng::genReal() * 0.1 - 0.05, Rng::genReal() * 0.1 - 0.05};
}

Point3D Genom::genSmallPosOffset() const
{
   return {Rng::genReal() * 0.01 - 0.005, Rng::genReal() * 0.01 - 0.005, Rng::genReal() * 0.01 - 0.005};
}

void Genom::updateNumNeurons()
{
   mNumNeurons = 0;

   unsigned int currentBlockSize = 0;
   for(auto& g : mGenes)
   {
      if(std::holds_alternative<SpawnNeuronGene>(g))
      {
         currentBlockSize++;
      }
      else if(std::holds_alternative<MirrorGene>(g) ||
         std::holds_alternative<CopyWithOffsetGene>(g))
      {
         currentBlockSize *= 2;  
      }
      else if(std::holds_alternative<PushGene>(g))
      {
         mNumNeurons += currentBlockSize;
      }
   }

   mNumNeurons += currentBlockSize;
}

void Genom::operator= (const Genom& other)
{
   if(other.mNumInputs != mNumInputs || other.mNumOutputs != mNumOutputs)
   {
      throw -1;
   }

   mNumNeurons = other.mNumNeurons;
   mGenes = other.mGenes;
}

}