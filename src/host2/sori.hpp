#pragma once
#include "IProject.hpp"
#include <filesystem>

namespace sori
{

std::unique_ptr<IProject> createProject(const boost::property_tree::ptree& config);
std::unique_ptr<IProject> loadProject(const std::filesystem::path& path, const boost::property_tree::ptree& config);

}

