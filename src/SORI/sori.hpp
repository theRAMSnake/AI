#pragma once

#include <list>
#include <map>
#include <thread>
#include "gacommon/IPlayground.hpp"
#include "gacommon/rng.hpp"
#include "task.hpp"
#include "pop.hpp"
#include "environment.hpp"

namespace sori
{

struct Config
{
   std::size_t populationSize = 250;
   double survivalRate = 0.4;

   std::size_t numThreads = 3;
   Size environmentSize = {100, 100};
};

class Sori
{
public:
   Sori(const Config& cfg);

   void step();
   void addTask(std::shared_ptr<ITask> task);

private:
   void select();
   void populate();
   void evaluate();

   std::list<Pop>::iterator selectTournament(std::list<Pop>& pops);

   Config mCfg;
   std::list<Pop> mPopulation;
   std::vector<std::shared_ptr<ITask>> mTasks;
   std::map<std::string, std::vector<double>> mTaskStatistics;
   std::size_t mGeneration = 1;
   std::size_t mCurrentEnergyLimit = 100;
};

}
