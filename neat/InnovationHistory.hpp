#pragma once
#include <map>

namespace neat
{

using NodeId = unsigned int;
using InnovationNumber = unsigned int;

class InnovationHistory
{
public:
    InnovationNumber get(const NodeId from, const NodeId to);

private:
    InnovationNumber mInnovationNumber = 0;
    std::map<NodeId, std::map<NodeId, InnovationNumber>> mAllConnections;
};

}