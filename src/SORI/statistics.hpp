#pragma once
#include <string>

namespace sori
{

class IStatistics
{
public:
    virtual ~IStatistics(){}

    virtual void onStepResult(const std::string& taskName, const std::size_t genNumber, const std::size_t energyLimit, const int maxScore, const int avgScore) = 0;
};

}
