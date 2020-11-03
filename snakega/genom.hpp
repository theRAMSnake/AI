#pragma once
#include <vector>
#include <set>
#include <variant>
#include "neuroevolution/IPlayground.hpp"
#include "neuroevolution/activation.hpp"

namespace snakega
{

class Genom
{
public:
   friend class GenomDecoder;
   Genom(const std::size_t numInputs, const std::size_t numOutputs);

   void operator= (const Genom& other);

   static Genom createMinimal(const std::size_t numInputs, const std::size_t numOutputs);

   void mutateStructure();
   void mutateParameters();
   void crossoverParameters(const Genom& other);

   unsigned int getNumNeurons() const;
   unsigned int getComplexity() const;
   unsigned int getNumInputs() const;
   unsigned int getNumOutputs() const;

   static Genom loadState(std::ifstream& s, const std::size_t numInputs, const std::size_t numOutputs);
   void saveState(std::ofstream& s) const;

private:
   const std::size_t mNumInputs;
   const std::size_t mNumOutputs;
};

}