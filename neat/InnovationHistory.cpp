#include "InnovationHistory.hpp"
#include <stdexcept>
#include <iostream>

namespace neat
{

InnovationNumber InnovationHistory::get(const NodeId from, const NodeId to)
{
    auto fromPos = mAllConnections.find(from);

    if(fromPos == mAllConnections.end())
    {
        mAllConnections[from][to] = mInnovationNumber;
        return mInnovationNumber++;
    }
    else
    {
        auto toPos = fromPos->second.find(to);
        if(toPos == fromPos->second.end())
        {
            fromPos->second.insert(std::make_pair(to, mInnovationNumber));
            return mInnovationNumber++;
        }

        return toPos->second;
    }
}

void InnovationHistory::saveState(std::ofstream& s)
{
    s.write((char*)&mInnovationNumber, sizeof(InnovationNumber));
    std::cout << "S_innovationNumber" << mInnovationNumber;
    for(auto x : mAllConnections)
    {
        for(auto y : x.second)
        {
            s.write((char*)&x.first, sizeof(NodeId));
            s.write((char*)&y.first, sizeof(NodeId));
            s.write((char*)&y.second, sizeof(InnovationNumber));
        }
    }
}

std::pair<NodeId, NodeId> InnovationHistory::get(const InnovationNumber n) const
{
    if(mCache.size() != 0)
    {
        return mCache.find(n)->second;
    }

    for(auto x : mAllConnections)
    {
        for(auto y : x.second)
        {
            if(y.second == n)
            {
                return std::make_pair(x.first, y.first);
            }
        }
    }

    throw std::invalid_argument("Innovation number not found");
}

void InnovationHistory::loadState(std::ifstream& s)
{
    mAllConnections.clear();

    s.read((char*)&mInnovationNumber, sizeof(InnovationNumber));
    std::cout << "L_innovationNumber" << mInnovationNumber;
    for(InnovationNumber i = 0; i < mInnovationNumber; ++i)
    {
        NodeId a;
        NodeId b;
        InnovationNumber numb;

        s.read((char*)&a, sizeof(NodeId));
        s.read((char*)&b, sizeof(NodeId));
        s.read((char*)&numb, sizeof(InnovationNumber));

        mAllConnections[a][b] = numb;
    }
}

void InnovationHistory::buildCache()
{
    for(auto x : mAllConnections)
    {
        for(auto y : x.second)
        {
            mCache.insert(std::make_pair(y.second, std::make_pair(x.first, y.first)));
        }
    }
}

void InnovationHistory::clearCache()
{
    mCache.clear();
}

}