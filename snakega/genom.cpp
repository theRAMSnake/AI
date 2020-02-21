#include "genom.hpp"
#include "neuroevolution/rng.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <numeric>
#include "logger/Logger.hpp"

namespace snakega
{

bool operator< (const Terminal& a, const Terminal& b)
{
    if (a.type == b.type)
    {
        return a.id < b.id;
    }
    else
    {
        return a.type < b.type;
    }
}

Genom::Genom(const std::vector<Point3D>& inputs, const std::vector<Point3D>& outputs)
: mNumInputs(inputs.size())
, mNumOutputs(outputs.size())
, mInputs(inputs)
, mOutputs(outputs)
{

}

NeuronId Genom::spawnNeuron()
{
    auto id = genNeuronId();
    mNeurons.push_back({
       genPos(),
       id,
       static_cast<ActivationFunctionType>(Rng::genChoise(NUM_ACTIVATION_FUNCTION_TYPES)),
       Rng::genReal() }
    );

    auto lottery = createConnectionLotery(mNeurons.back());

    for (unsigned int x = 0; x < Rng::genChoise(5) && lottery.hasDst(); ++x)
    {
        addConnection(
            { TerminalType::Neuron, id },
            lottery.pickDst(),
            Rng::genWeight()
        );
    }

    for (unsigned int y = 0; y < Rng::genChoise(5) && lottery.hasSrc(); ++y)
    {
        addConnection(
            lottery.pickSrc(),
            { TerminalType::Neuron, id },
            Rng::genWeight()
        );
    }

    return id;
}

Genom Genom::createHalfConnected(const std::vector<Point3D>& inputs, const std::vector<Point3D>& outputs)
{
   Genom result(inputs, outputs);

   result.spawnNeuron();

   return result;
}

bool operator == (Terminal A, Terminal B)
{
    return A.type == B.type && A.id == B.id;
}

bool operator != (Terminal A, Terminal B)
{
    return !(A == B);
}

void Genom::addConnection(Terminal A, Terminal B, double weight)
{
    if (std::find_if(mConnections.begin(), mConnections.end(), [&](auto x) {return x.A == A && x.B == B;}) == mConnections.end())
    {
        mConnections.push_back({A, B, weight});
    }
}

void Genom::mutateStructure(const MutationConfig& mutationConfig)
{
   //Do exactly one change: AddConnection/RemoveConnection/RemoveRedundantNeuron/AddNeuron
   switch (Rng::genChoise(4))
   {
   case 0:
        if (!mNeurons.empty())
        {
            auto& n = mNeurons[Rng::genChoise(mNeurons.size())];
            auto lottery = createConnectionLotery(n);

            bool isSrc = Rng::genProbability(0.5);
            if (isSrc)
            {
                if (lottery.hasSrc())
                {
                    addConnection(
                        lottery.pickSrc(),
                        { TerminalType::Neuron, n.id },
                        Rng::genWeight()
                    );
                }
            }
            else
            {
                if (lottery.hasDst())
                {
                    addConnection(
                        { TerminalType::Neuron, n.id },
                        lottery.pickDst(),
                        Rng::genWeight()
                    );
                }
            }
        }
       break;

   case 1:
       if (!mConnections.empty())
       {
          mConnections.erase(mConnections.begin() + Rng::genChoise(mConnections.size()));
       }
       break;

   case 2:
       spawnNeuron();
       break;

   case 3:
       if (!mNeurons.empty())
       {
           auto pos = mNeurons.begin() + Rng::genChoise(mNeurons.size());

           auto numLinks = std::accumulate(mConnections.begin(), mConnections.end(), 0, [&](auto c, auto x) {
               if ((x.A.id == pos->id && x.A.type == TerminalType::Neuron) ||
                   (x.B.id == pos->id && x.B.type == TerminalType::Neuron))
               {
                   return c + 1;
               }

               return c;
           });

           if (numLinks < 2)
           {
               mConnections.erase(std::remove_if(mConnections.begin(), mConnections.end(), [&](auto x) {
                   return (x.A.id == pos->id && x.A.type == TerminalType::Neuron) ||
                       (x.B.id == pos->id && x.B.type == TerminalType::Neuron); }),
                   mConnections.end());

               mNeurons.erase(pos);
           }
       }
       break;

   default:
       break;
   }
}

double calculateDistance(const Point3D& srcPos, const Point3D& otherPos)
{
    return sqrt((srcPos.x - otherPos.x) * (srcPos.x - otherPos.x) +
        (srcPos.y - otherPos.y) * (srcPos.y - otherPos.y) +
        (srcPos.z - otherPos.z) * (srcPos.z - otherPos.z));
}

bool has(const std::vector<Terminal>& vec, const Terminal& t)
{
    return std::find(vec.begin(), vec.end(), t) != vec.end();
}

ConnectionLotery Genom::createConnectionLotery(const SpawnNeuronGene& g) const
{
    ConnectionLotery result;

    std::vector<Terminal> srcConnections;
    std::vector<Terminal> dstConnections;

    for (auto& c : mConnections)
    {
        if (c.B == Terminal{ TerminalType::Neuron, g.id })
        {
            srcConnections.push_back(c.B);
        }
        if (c.A == Terminal{ TerminalType::Neuron, g.id })
        {
            dstConnections.push_back(c.A);
        }
    }

    for (std::size_t i = 0; i < mInputs.size(); ++i)
    {
        auto otherPos = mInputs[i];
        auto distance = calculateDistance(g.pos, otherPos);

        if (!has(srcConnections, { TerminalType::Input, TerminalId(i)}))
        {
            result.addSrc({ TerminalType::Input, TerminalId(i) }, distance);
        }
    }

    for (std::size_t i = 0; i < mOutputs.size(); ++i)
    {
        auto otherPos = mOutputs[i];
        auto distance = calculateDistance(g.pos, otherPos);

        if (!has(dstConnections, { TerminalType::Output, TerminalId(i) }))
        {
            result.addDst({ TerminalType::Output, TerminalId(i) }, distance);
        }
    }

    for (auto& n : mNeurons)
    {
        auto otherPos = n.pos;
        auto distance = calculateDistance(g.pos, otherPos);

        if (!has(dstConnections, { TerminalType::Neuron, n.id }))
        {
            result.addDst({ TerminalType::Neuron, n.id }, distance);
        }

        if (!has(srcConnections, { TerminalType::Neuron, n.id }))
        {
            result.addSrc({ TerminalType::Neuron, n.id }, distance);
        }
    }

    return result;
}

void Genom::mutateParameters(const MutationConfig& mutationConfig)
{
   for(std::size_t i = 0; i < mConnections.size(); ++i)
   {
       if (Rng::genProbability(0.2))
       {
           mConnections[i].weight = Rng::genWeight();
       }
       else
       {
           mConnections[i].weight += Rng::genPerturbation();
       }
   }
}

void Genom::crossoverParameters(const Genom& other)
{
    if (mConnections.size() != other.mConnections.size())
    {
        throw - 1;
    }

    for (std::size_t i = 0; i < mConnections.size(); ++i)
    {
        mConnections[i].weight = (mConnections[i].weight + other.mConnections[i].weight) / 2;
    }
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

void ConnectionLotery::addSrc(const Terminal& t, const double distance)
{
    mSrc.insert({distance, t});
}

void ConnectionLotery::addDst(const Terminal& t, const double distance)
{
    mDst.insert({ distance, t });
}

bool ConnectionLotery::hasSrc()
{
    return !mSrc.empty();
}

bool ConnectionLotery::hasDst()
{
    return !mDst.empty();
}

Terminal ConnectionLotery::pickSrc()
{
    while (true)
    {
        for (auto iter = mSrc.begin(); iter != mSrc.end(); ++iter)
        {
            if (Rng::genProbability(0.25))
            {
                auto t = iter->second;
                mSrc.erase(iter);
                return t;
            }
        }
    }
}

Terminal ConnectionLotery::pickDst()
{
    while (true)
    {
        for (auto iter = mDst.begin(); iter != mDst.end(); ++iter)
        {
            if (Rng::genProbability(0.25))
            {
                auto t = iter->second;
                mDst.erase(iter);
                return t;
            }
        }
    }
}

Terminal Genom::genRandomTerminal(const bool isSrc) const
{
    Terminal result;

    if (isSrc && Rng::genProbability(mNumInputs / (double)(mNumInputs + mNeurons.size())))
    {
        result.type = TerminalType::Input;
        result.id = Rng::genChoise(mNumInputs);
    }
    else if (!isSrc && Rng::genProbability(mNumOutputs / (double)(mNumOutputs + mNeurons.size())))
    {
        result.type = TerminalType::Output;
        result.id = Rng::genChoise(mNumOutputs);
    }
    else
    {
        result.type = TerminalType::Neuron;
        result.id = Rng::genChoise(mNeurons.size());
    }

    return result;
}

}