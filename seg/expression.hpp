#pragma once
#include "variant"

namespace seg
{

struct RandomNumber{};
struct AlwaysZero{};
struct MemAddress { unsigned int addr;};
struct InputAddress { unsigned int addr;};
using VAL = std::variant<double, MemAddress, InputAddress, RandomNumber, AlwaysZero>;

enum class FuncCategory {Primitive, Domain, Extension};
struct FUNC
{
    FuncCategory category;
    unsigned int id;
    VAL params[4];
};

using Expression = std::variant<VAL, FUNC>;

}