#pragma once
#include <boost/property_tree/ptree.hpp>
#include "gacommon/IPlayground.hpp"

using PopId = std::size_t;
struct PopInfo
{
    PopId id;
    gacommon::Fitness fitness;
    std::size_t complexity;
};

class IProject
{
public:
    virtual ~IProject() {}

    virtual boost::property_tree::ptree getConfig() const = 0;
    virtual void reconfigure(const boost::property_tree::ptree& cfg) = 0;
    virtual void step() = 0;
    virtual void saveState(const std::string& fileName) = 0;
    virtual gacommon::Fitness getBestFitness() const = 0;
    virtual std::size_t getGenerationNumber() const = 0;
    virtual double getAverageComplexity() const = 0;

    virtual std::vector<PopInfo> describePopulation() const = 0;
    virtual std::vector<std::byte> exportIndividual(const PopId id) const = 0;
    virtual std::string perform(const PopId& id) const = 0;
};
