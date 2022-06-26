#pragma once
#include "gacommon/IPlayground.hpp"
#include "Project.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

class IEngine
{
public:
    virtual ~IEngine(){}

    virtual std::unique_ptr<IProject> createProject(std::unique_ptr<gacommon::IPlayground>&& playground, const boost::property_tree::ptree cfg) = 0;
    virtual std::unique_ptr<IProject> loadProject(std::unique_ptr<gacommon::IPlayground>&& playground, const boost::property_tree::ptree cfg, const std::string& filename) = 0;
};

class NeatEngine : public IEngine
{
public:
    std::unique_ptr<IProject> createProject(std::unique_ptr<gacommon::IPlayground>&& playground, const boost::property_tree::ptree cfg) override;
    std::unique_ptr<IProject> loadProject(std::unique_ptr<gacommon::IPlayground>&& playground, const boost::property_tree::ptree cfg, const std::string& filename) override;
};

class Snake4Engine : public IEngine
{
public:
    std::unique_ptr<IProject> createProject(std::unique_ptr<gacommon::IPlayground>&& playground, const boost::property_tree::ptree cfg) override;
    std::unique_ptr<IProject> loadProject(std::unique_ptr<gacommon::IPlayground>&& playground, const boost::property_tree::ptree cfg, const std::string& filename) override;
};
