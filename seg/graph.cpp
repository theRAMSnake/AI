#include "graph.hpp"
#include <algorithm>
#include <iostream>
#include <neuroevolution/rng.hpp>

namespace seg
{

Graph::Graph(const unsigned int numCommands)
{
   Choise rootChoise;
   rootChoise.selector = RandomEven{};

   for(unsigned int i = 0; i < numCommands; ++i)
   {
      rootChoise.options.push_back(i + 1);
   }

   mNodes.push_back({});
   mNodes.back().id = mNextNodeId++;
   mNodes.back().payload = rootChoise;

   for(unsigned int i = 0; i < numCommands; ++i)
   {
      mNodes.push_back({});
      mNodes.back().id = mNextNodeId++;
      mNodes.back().payload = Action(Result{i});
   }
}

std::size_t Graph::size() const
{
   return mNodes.size();
}

const GraphNode& Graph::root() const
{
   return mNodes[0];
}

const GraphNode& Graph::get(const NodeId id) const
{
   return mNodes[id];
}

GraphNode& Graph::root()
{
   return mNodes[0];
}

GraphNode& Graph::get(const NodeId id)
{
   return *std::find_if(mNodes.begin(), mNodes.end(), [=](auto x){return x.id == id;});
}

GraphNode& Graph::randomNode()
{
   return mNodes[Rng::genChoise(mNodes.size())];
}

NodeId Graph::addNode(const Payload& payload)
{
   mNodes.push_back({});
   mNodes.back().id = mNextNodeId++;
   mNodes.back().payload = payload;

   return mNodes.back().id;
}

void Graph::removeIfUnreferenced(const NodeId id)
{
   for(auto& n : mNodes)
   {
      if(std::holds_alternative<Choise>(n.payload))
      {
         auto& options = std::get<Choise>(n.payload).options;
         if(std::find_if(options.begin(), options.end(), [=](auto x){return x == id;}) !=
            options.end())
         {
            return;
         }
      }
      else
      {
         auto& action = std::get<Action>(n.payload);
         if(std::holds_alternative<Operation>(action))
         {
            if(std::get<Operation>(action).next == id)
            {
               return;
            }
         }
      }
   }

   mNodes.erase(std::find_if(mNodes.begin(), mNodes.end(), [=](auto x){return x.id == id;}));
}

void Graph::fixReferences(const NodeId from, const NodeId to)
{
   for(auto& n : mNodes)
   {
      if(std::holds_alternative<Choise>(n.payload))
      {
         auto& options = std::get<Choise>(n.payload).options;
         std::replace(options.begin(), options.end(), from, to);
      }
      else
      {
         auto& action = std::get<Action>(n.payload);
         if(std::holds_alternative<Operation>(action))
         {
            if(std::get<Operation>(action).next == from)
            {
               std::get<Operation>(action).next = to;
            }
         }
      }
   }
}

std::string to_string(const Expression& ex)
{
   return "EXP?";
}

std::string to_string(const IfSign& ex)
{
   return "?";
}


void Graph::print() const
{
   for(auto& n : mNodes)
   {
      std::cout << n.id << ": ";

      if(std::holds_alternative<Choise>(n.payload))
      {
         auto& ch = std::get<Choise>(n.payload);
         auto& options = ch.options;

         if(std::holds_alternative<RandomEven>(ch.selector))
         {
            std::cout << "Random choise: ";
         }
         else if(std::holds_alternative<If>(ch.selector))
         {
            auto& if_ch = std::get<If>(ch.selector);
            std::cout << "IF(" + to_string(if_ch.A) + to_string(if_ch.sign) + to_string(if_ch.B) + ") choise: ";
         }
         else if(std::holds_alternative<Compare>(ch.selector))
         {
            auto& cmpch = std::get<Compare>(ch.selector);
            std::cout << "CMP(" + to_string(cmpch.A) + ", " + to_string(cmpch.B) + ") choise: ";
         }
         else if(std::holds_alternative<Switch>(ch.selector))
         {
            auto& sw = std::get<Switch>(ch.selector);
            std::cout << "Switch(?)";
         }   
         else if(std::holds_alternative<RandomWeighted>(ch.selector))
         {
            auto& rw = std::get<RandomWeighted>(ch.selector);
            std::cout << "RandomWeighted(?)";
         }

         for(auto o : options)
         {
            std::cout << o << " ";
         }
      }
      else
      {
         auto& action = std::get<Action>(n.payload);
         if(std::holds_alternative<Operation>(action))
         {
            auto& op = std::get<Operation>(action);
            std::cout << "operation([" << op.addr.addr << "] = " << to_string(op.expr) << ")";
         }
         else
         {
            auto& r = std::get<Result>(action);
            std::cout << "result = " << r.commandId;
         }
      }

      std::cout << std::endl;
   }
}

}