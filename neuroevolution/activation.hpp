#pragma once

#include <cmath>
#include <algorithm>

enum class ActivationFunctionType
{
    SIGMOID,
    TANH,
    SIN,
    GAUSS,
    RELU,
    ELU,
    LELU,
    SELU,
    IDENTITY,
    CLAMPED,
    LOG,
    EXP,
    ABS,
    SQUARE,
    CUBE,
    GELU
};

const int NUM_ACTIVATION_FUNCTION_TYPES = 16;

using ActivationFunction = double (*)(double);

double sigmoid (const double val);

double gauss (const double val);

double relu (const double val);

double elu (const double val);

double lelu (const double val);

double selu (const double val);

double identity(const double val);

double clamped(const double val);

double square(const double val);

double cube(const double val);

double gelu(const double val);

ActivationFunction getPtr(const ActivationFunctionType type);