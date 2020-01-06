#include "activation.hpp"

double sigmoid (const double val)
{
   if(val > 1.5)
   {
      return 1.0;
   }
   else if(val < -1.5)
   {
      return 0;
   }
   else
   {
      return 1.0 / (1 + std::exp(-5.0 * val));
   }
}

double gauss (const double val)
{
    return std::exp(-5.0 * val * val);
}

double relu (const double val)
{
    return std::max(0.0, val);
}

double elu (const double val)
{
    if(val > 0)
    {
        return val;
    }
    else
    {
        return std::exp(val) - 1;
    }
}

double lelu (const double val)
{
    if(val > 0)
    {
        return val;
    }
    else
    {
        return val * 0.005;
    }
}

double selu (const double val)
{
    auto lam = 1.0507009873554804934193349852946;
    auto alpha = 1.6732632423543772848170429916717;

    if(val > 0)
    {
        return lam * val;
    }
    else
    {
        return lam * alpha * (std::exp(val) - 1);
    }
}

double identity(const double val)
{
    return val;
}

double clamped(const double val)
{
    return std::min(1.0, std::max(-1.0, val));
}

double square(const double val)
{
    return val * val;
}

double cube(const double val)
{
    return val * val * val;
}

double gelu(const double val)
{
    return 0.5 * val * (1 + tanh(sqrt(M_2_PI) * (val + 0.044715 * val * val * val)));
}

ActivationFunction getPtr(const ActivationFunctionType type)
{
    switch(type)
    {
        case ActivationFunctionType::SIGMOID: return sigmoid;
        case ActivationFunctionType::TANH: return tanh;
        case ActivationFunctionType::SIN: return sin;
        case ActivationFunctionType::GAUSS: return gauss;
        case ActivationFunctionType::RELU: return relu;
        case ActivationFunctionType::ELU: return elu;
        case ActivationFunctionType::LELU: return lelu;
        case ActivationFunctionType::SELU: return selu;
        case ActivationFunctionType::IDENTITY: return identity;
        case ActivationFunctionType::CLAMPED: return clamped;
        case ActivationFunctionType::LOG: return log;
        case ActivationFunctionType::EXP: return std::exp;
        case ActivationFunctionType::ABS: return std::abs;
        case ActivationFunctionType::SQUARE: return square;
        case ActivationFunctionType::CUBE: return cube;
        case ActivationFunctionType::GELU: return gelu;
    }

    throw -1;
}