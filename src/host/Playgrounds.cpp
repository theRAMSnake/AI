#include "Playgrounds.hpp"
#include "playgrounds/LogicalPG.hpp"
#include "playgrounds/CalculatorPG.hpp"

std::unique_ptr<gacommon::IPlayground> createPlayground(const std::string& name)
{
    if(name == "logical")
    {
        return std::make_unique<pgs::LogicalPG>();
    }
    else if(name == "calc")
    {
        return std::make_unique<pgs::CalculatorPG>();
    }
    else
    {
        throw std::runtime_error("Unknown playground: " + name);
    }
}
