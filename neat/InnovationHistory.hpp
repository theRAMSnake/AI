#pragma once
#include <map>
#include <fstream>
#include <unordered_map>

namespace neat
{

using NodeId = unsigned int;
using InnovationNumber = unsigned int;

class InnovationHistory
{
public:
    InnovationNumber get(const NodeId from, const NodeId to);
    std::pair<NodeId, NodeId> get(const InnovationNumber n) const;

    void saveState(std::ofstream& s);
    void loadState(std::ifstream& s);

    void buildCache();
    void clearCache();

private:
    InnovationNumber mInnovationNumber = 0;
    std::map<NodeId, std::map<NodeId, InnovationNumber>> mAllConnections;
    std::unordered_map<InnovationNumber, std::pair<NodeId, NodeId>> mCache;
};

}