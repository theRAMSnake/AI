#include "genom.hpp"
#include "neuroevolution/rng.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <numeric>
#include "logger/Logger.hpp"

namespace snakega
{

Genom::Genom(const std::size_t numInputs, const std::size_t numOutputs)
: mNumInputs(numInputs)
, mNumOutputs(numOutputs)
{

}

void Genom::mutateStructure()
{
   
}

void Genom::mutateParameters()
{
   
}

void Genom::crossoverParameters(const Genom& other)
{
    
}

void Genom::operator= (const Genom& other)
{
   if(other.mNumInputs != mNumInputs || other.mNumOutputs != mNumOutputs)
   {
      throw -1;
   }
}

Genom Genom::createMinimal(const std::size_t numInputs, const std::size_t numOutputs)
{
    throw -1;
}

unsigned int Genom::getNumNeurons() const
{
   return 0;
}

unsigned int Genom::getComplexity() const
{
   return 0;
}

unsigned int Genom::getNumInputs() const
{
   return mNumInputs;
}

unsigned int Genom::getNumOutputs() const
{
   return mNumOutputs;
}

Genom Genom::loadState(std::ifstream& s, const std::size_t numInputs, const std::size_t numOutputs)
{
   
}

void Genom::saveState(std::ofstream& s) const
{
   
}

}