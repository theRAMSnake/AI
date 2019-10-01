#include "InnovationHistory.hpp"

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

}