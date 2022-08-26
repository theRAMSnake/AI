#pragma once
#include <boost/property_tree/ptree_fwd.hpp>
#include <vector>
#include <memory>
#include "system.hpp"
#include "gacommon/IPlayground.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace snake4
{

class Pop
{
public:
   Pop();

   Pop& operator = (const Pop& other);

   Pop cloneMutated(const gacommon::IODefinition& io) const;
   static Pop createMinimal(const gacommon::IODefinition& io);
   std::unique_ptr<gacommon::IAgent> createAgent(const gacommon::IODefinition& io) const;

   void saveState(boost::property_tree::ptree& ar) const;
   static Pop loadState(const boost::property_tree::ptree& ar);

   std::vector<BlockDefinition> blocks;
   gacommon::Fitness fitness = 0;
};

}
