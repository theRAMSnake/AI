#pragma once

#include <list>
#include <map>
#include <thread>
#include "gacommon/IPlayground.hpp"
#include "gacommon/rng.hpp"
#include "task.hpp"
#include "pop.hpp"
#include "environment.hpp"
#include "database.hpp"
#include "statistics.hpp"

namespace sori
{

struct Config
{
   bool testMode = false;
   std::size_t populationSize = 250;
   double survivalRate = 0.25;

   std::size_t numThreads = 4;
   dng::Size environmentSize = {500, 364};
};

class Sori
{
public:
   Sori(const Config& cfg, ITaskManager& taskManager);
   Sori(ITaskManager& taskManager, Database& db);

   void step();
   void addTask(std::shared_ptr<ITask> task);
   void reconfigure(const Config& cfg);
   void setStatistics(IStatistics& stats);

   const Pop& getTopPerformer() const;
   std::size_t getEnergyLimit() const;
   std::size_t getGeneration() const;
   Config getConfig() const;
   const std::list<Pop>& getPopulation() const;
   int getLastTaskScore(const std::string& taskName) const;

   void checkpoint(Database& db);

private:
   void select();
   void populate();
   void evaluate();

   std::list<Pop>::iterator selectTournament(std::list<Pop>& pops);

   Config mCfg;
   std::list<Pop> mPopulation;
   ITaskManager& mTaskManager;
   TaskScores mGlobalTaskScores;
   std::size_t mGeneration = 1;
   std::size_t mCurrentEnergyLimit = 100;

   std::optional<std::reference_wrapper<IStatistics>> mStats;
};

}
