#pragma once
#include "system.hpp"

namespace snake4
{

ActivatorDefinition generateRandomSensor(const gacommon::IODefinition& def);
ForceDefinition generateRandomManipulator(const gacommon::IODefinition& def);

class IMutationObserver
{
public:
    virtual void onMinorTweak(const std::size_t idx) = 0;
    virtual void onMajorTweak(const std::size_t idx) = 0;
    virtual void onReplaceForce(const std::size_t idx) = 0;
    virtual void onReplaceActivator(const std::size_t idx) = 0;
    virtual void onInsertNode(const std::size_t idx) = 0;
    virtual void onCloneNode(const std::size_t idx) = 0;
    virtual void onDestroyNode() = 0;
};

std::vector<BlockDefinition> mutate(const gacommon::IODefinition& def, const std::vector<BlockDefinition>& input, IMutationObserver& observer);
std::vector<BlockDefinition> mutate(const gacommon::IODefinition& def, const std::vector<BlockDefinition>& input);

}
