#include "neat/neuro_net.hpp"
#include "NumberCmpPG.hpp"
#include "NeatController.hpp"

#include <iostream>
#include <memory>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <boost/random.hpp>

void printGenom(const neat::Genom& g, const bool includeWeights = false)
{
   std::cout << "         (";

   for(auto& x : g)
   {
      if(x.enabled)
      {
         if(includeWeights)
         {
            std::cout << x.srcNodeId << "->" << x.dstNodeId << ":" << std::setprecision(2) << x.weight << " ";
         }
         else
         {
            std::cout << x.srcNodeId << "->" << x.dstNodeId << " ";
         }
      }
   }

   std::cout << ")" << std::endl;
}

void printState(NeatController& c)
{
   auto& pops = c.getPopulation();

   std::cout << "Generation: " << c.getGeneration() << std::endl;
   std::cout << "Total population: " << pops.size() << std::endl;
   std::cout << "Num species: " << pops.numSpecies() << std::endl;
   std::cout << "Average fitness: " << pops.getAverageFitness() << std::endl;
   std::cout << "Best of each specie: " << std::endl;

   for(auto& s : pops)
   {
      auto best = std::max_element(s.population.begin(), s.population.end(), [](auto x, auto y){return x.fitness < y.fitness;});
      std::cout << s.id << ": Fitness: " << best->fitness << std::endl;
      printGenom(best->genotype);
   }
}

bool quitRequested = false;

template<class T>
T requestParam(const std::string& name)
{
   std::cout << name << ": ";
   T p = T();
   std::cin >> p;
   std::cin.get();

   return p;
}

void exportToFile(const neat::Genom& g, const std::string& filename)
{
   std::ofstream f;
   f.open(filename, std::ios_base::out | std::ios_base::trunc);
   for(auto& x : g)
   {
      f << x.innovationNumber << ":" << (x.enabled ? "+" : "-") << " " << x.srcNodeId << "->" << x.dstNodeId << " " << x.weight << std::endl;
   }
   
   f.close();
}

neat::Genom importFromFile(const std::string& filename, const neat::NodeId numInputs, const neat::NodeId numOutputs)
{
   neat::Genom result(numInputs, numOutputs);

   std::ifstream f;
   f.open(filename, std::ios_base::in);
   if (f.is_open())
   {
      std::string str;
      while ( std::getline (f, str) )
      {
         std::istringstream s(str);
         char spare;
         neat::Gene g;

         s >> g.innovationNumber;
         s >> spare;
         s >> spare;

         g.enabled = spare == '+';

         s >> g.srcNodeId;
         s >> spare;
         s >> spare;
         s >> g.dstNodeId;
         s >> g.weight;

         result += g;
      }

      f.close();
   }
   
   return result;
}

//-----------------------------------------------------------------------------
using CommandFunction = std::function<void(IPlayground&, NeatController&)>;

void quitCommand(IPlayground& p, NeatController& c)
{
   quitRequested = true;
}

void stepCommand(IPlayground& p, NeatController& c)
{
   p.step();
   c.step();

   printState(c);
}

void advanceCommand(IPlayground& p, NeatController& c)
{
   auto g = requestParam<unsigned int>("generations");

   for(unsigned int i = 0; i < g; ++i)
   {
      p.step();
      c.step();

      std::cout << ".";
      std::cout.flush();
   }

   std::cout << std::endl;
   printState(c);
}

void reachCommand(IPlayground& p, NeatController& c)
{
   auto f = requestParam<neat::Fitness>("fitness");

   bool stop = false;
   while(!stop)
   {
      p.step();
      c.step();

      auto& pops = c.getPopulation();
      for(auto& s : pops)
      {
         auto best = std::max_element(s.population.begin(), s.population.end(), [](auto x, auto y){return x.fitness < y.fitness;});
         if(best->fitness >= f)
         {
            stop = true;
            break;
         }
      }

      std::cout << ".";
      std::cout.flush();
   }

   std::cout << std::endl;
   printState(c);
}

void viewCommand(IPlayground& p, NeatController& c)
{
   auto sId = requestParam<unsigned int>("specie");

   auto& pops = c.getPopulation();
   for(auto& s : pops)
   {
      if(s.id == sId)
      {
         int popNum = 0;
         for(auto& p : s.population)
         {
            std::cout << popNum++ << ": Fitness: " << p.fitness << std::endl;
            printGenom(p.genotype, true/*Include weights*/);
         }
         break;
      }
   }
}

void exportCommand(IPlayground& p, NeatController& c)
{
   auto sId = requestParam<unsigned int>("specie");
   auto fname = requestParam<std::string>("filename");

   auto& pops = c.getPopulation();
   for(auto& s : pops)
   {
      if(s.id == sId)
      {
         auto best = std::max_element(s.population.begin(), s.population.end(), [](auto x, auto y){return x.fitness < y.fitness;});
         exportToFile(best->genotype, fname);

         break;
      }
   }
}

void playCommand(IPlayground& p, NeatController& c)
{
   try
   {
      auto fname = requestParam<std::string>("filename");
      auto genom = importFromFile(fname, p.getConfig().numInputs, p.getConfig().numOutputs);

      printGenom(genom, true);
      
      p.play(genom);
   }
   catch(...)
   {
      std::cout << "File is invalid" << std::endl;
   }
}

//--------------------------------------------------------------

int main()
{
   std::cout << "Neat shell" << std::endl;

   NumberCmpPG playground;
   NeatController neatController(playground);

   std::map<std::string, CommandFunction> commands;

   commands["quit"] = quitCommand;
   commands[""] = stepCommand;
   commands["advance"] = advanceCommand;
   commands["reach"] = reachCommand;
   commands["view"] = viewCommand;
   commands["export"] = exportCommand;
   commands["play"] = playCommand;

   char buf[300];
   while(std::cin.getline(buf, 300))
   {
      auto t1 = std::chrono::high_resolution_clock::now();
      std::string s = buf;

      auto cmdPos = commands.find(s);
      if(cmdPos != commands.end())
      {
         cmdPos->second(playground, neatController);

         if(quitRequested)
         {
            return 0;
         }
      }
      else
      {
         std::cout << "Unknown command" << std::endl;
      }

      auto t2 = std::chrono::high_resolution_clock::now();
      auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();

      std::cout << "Executed: " << duration << "ms" << std::endl;
      std::cout << "-----------------------------------------------------------" << std::endl;
   }
}