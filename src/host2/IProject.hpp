#pragma once
#include <boost/property_tree/ptree.hpp>

class IProject
{
public:
    virtual ~IProject() {}

    //Callable on stoped project only
    virtual boost::property_tree::ptree getConfig() const = 0;
    virtual void reconfigure(const boost::property_tree::ptree& cfg) = 0;
    virtual void step() = 0;
    virtual void saveState(const std::string& fileName) = 0;
    virtual std::size_t getGenerationNumber() const = 0;

    //Callable any time
    virtual std::string printRecentStats() const = 0;
};
