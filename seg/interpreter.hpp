#pragma once
#include "expression.hpp"
#include <functional>
#include <map>

namespace seg
{

class Memory
{
public:
    Memory(const unsigned int slots);

    unsigned int getNumSlots() const;
    double at(const unsigned int address) const;

    void put(const unsigned int address, const double value);

private:
    std::vector<double> mSlots;
};

using FunctionImpl = std::function<double(double[4])>;
struct Function
{
    unsigned int numParams;
    FunctionImpl impl;
};

class FunctionLibrary
{
public:
    void add(const unsigned int id, Function f);

    unsigned int getNumFunctions() const;
    Function get(const unsigned int id) const;

private:
    std::map<unsigned int, Function> mFuncs;
};

class Interpreter
{
public:
    Interpreter(
        const Memory& memory, 
        const double* inputs,
        const FunctionLibrary& primitives, 
        const FunctionLibrary& extensions, 
        const FunctionLibrary& domainFunctions
        );

    double resolve(const Expression& exp) const;

private:
    double resolveValue(const VAL& val) const;
    double resolveFunc(const FUNC& func) const;

    const Memory& mMemory;
    const double* mInputs;
    const FunctionLibrary& mPrimitives;
    const FunctionLibrary& mExtensions;
    const FunctionLibrary& mDomainFunctions;
};

}