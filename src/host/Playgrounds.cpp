#include "Playgrounds.hpp"
#include "playgrounds/LogicalPG.hpp"

std::unique_ptr<gacommon::IPlayground> createPlayground(const std::string& name)
{
    if(name == "logical")
    {
        return std::make_unique<pgs::LogicalPG>();
    }
    else
    {
        throw std::runtime_error("Unknown playground: " + name);
    }
}
