#include "functions.hpp"
#include <cmath>
#include <neuroevolution/rng.hpp>

namespace seg
{

double add(double p[4])
{
    return p[0] + p[1];
}

double sub(double p[4])
{
    return p[0] - p[1];
}

double mult(double p[4])
{
    return p[0] * p[1];
}

double div(double p[4])
{
    return p[1] != 0 ? p[0] / p[1] : 0.0;
}

double mod(double p[4])
{
    return int(p[1]) != 0 ? int(p[0]) % int(p[1]) : 0.0;
}

double and_func(double p[4])
{
    return int(p[0]) & int(p[1]);
}

double or_func(double p[4])
{
    return int(p[0]) | int(p[1]);
}

double xor_func(double p[4])
{
    return int(p[0]) ^ int(p[1]);
}

double not_func(double p[4])
{
    return ~int(p[0]);
}

FunctionLibrary createPrimitivesLibrary()
{
    FunctionLibrary result;

    result.add(0, {2, add});
    result.add(1, {2, sub});
    result.add(2, {2, mult});
    result.add(3, {2, div});
    result.add(4, {2, mod});
    result.add(5, {2, and_func});
    result.add(6, {2, or_func});
    result.add(7, {2, xor_func});
    result.add(8, {1, not_func});

    return result;
}


double max(double p[4])
{
    return std::max(p[0], p[1]);
}

double min(double p[4])
{
    return std::min(p[0], p[1]);
}

double sin(double p[4])
{
    return std::sin(p[0]);
}

double rand_number(double p[4])
{
    return Rng::genWeight();
}

FunctionLibrary createExtensionLibrary()
{
    FunctionLibrary result;

    result.add(0, {2, max});
    result.add(1, {2, min});
    result.add(2, {1, sin});
    result.add(3, {0, rand_number});

    return result;
}

}