#include "interpreter.hpp"
#include "neuroevolution/rng.hpp"

namespace seg
{

Interpreter::Interpreter(
    const Memory& memory,
    const double* inputs,
    const FunctionLibrary& primitives, 
    const FunctionLibrary& extensions, 
    const FunctionLibrary& domainFunctions
    )
: mMemory(memory)
, mInputs(inputs)
, mPrimitives(primitives)
, mExtensions(extensions)
, mDomainFunctions(domainFunctions)
{

}

double Interpreter::resolve(const Expression& exp) const
{
    if(std::holds_alternative<VAL>(exp))
    {
        auto& val = std::get<VAL>(exp);

        return resolveValue(val);
    }
    else if(std::holds_alternative<FUNC>(exp))
    {
        auto& func = std::get<FUNC>(exp);

        return resolveFunc(func);
    }
    else
    {
        throw -1;
    }
}

double Interpreter::resolveValue(const VAL& val) const
{
    if(std::holds_alternative<double>(val))
    {
        return std::get<double>(val);
    }
    else if(std::holds_alternative<MemAddress>(val))
    {
        auto addr = std::get<MemAddress>(val);
        if(addr.addr < mMemory.getNumSlots())
        {
            return mMemory.at(addr.addr);
        }
        else
        {
            throw -1;
        }
    }
    else if(std::holds_alternative<InputAddress>(val))
    {
        auto addr = std::get<InputAddress>(val);
        return mInputs[addr.addr];
    }
    else if(std::holds_alternative<RandomNumber>(val))
    {
        return Rng::genReal();
    }
    else if(std::holds_alternative<AlwaysZero>(val))
    {
        return 0;
    }
    else
    {
        throw -1;
    }
}

double Interpreter::resolveFunc(const FUNC& func) const
{
    const FunctionLibrary* lib = nullptr;
    switch(func.category)
    {
        case FuncCategory::Primitive:
            lib = &mPrimitives;
            break;

        case FuncCategory::Extension:
            lib = &mExtensions;
            break;

        case FuncCategory::Domain:
            lib = &mDomainFunctions;
            break;
    }

    if(lib != nullptr)
    {
        if(func.id < lib->getNumFunctions())
        {
            auto f = lib->get(func.id);
            double params[4];

            for(unsigned int i = 0; i < f.numParams; ++i)
            {
                params[i] = resolveValue(func.params[i]);
            }

            return f.impl(params);
        }
    }
    else
    {
        throw -1;
    }

    return 0;
}

void FunctionLibrary::add(const unsigned int id, Function f)
{
    mFuncs[id] = f;
}

unsigned int FunctionLibrary::getNumFunctions() const
{
    return mFuncs.size();
}

Function FunctionLibrary::get(const unsigned int id) const
{
    return mFuncs.find(id)->second;
}

Memory::Memory(const unsigned int slots)
{
    mSlots.resize(slots, 0.0);
}

unsigned int Memory::getNumSlots() const
{
    return mSlots.size();
}

double Memory::at(const unsigned int address) const
{
    return mSlots[address];
}

void Memory::put(const unsigned int address, const double value)
{
    mSlots[address] = value;
}

}