#include "sori.hpp"

namespace sori
{

Sori::Sori(const Config& cfg, ITaskManager& taskManager)
: mCfg(cfg)
, mTaskManager(taskManager)
{
  for(std::size_t i = 0; i < mCfg.populationSize; ++i)
  {
     mPopulation.push_back(Pop::createMinimal());
  }
}

Sori::Sori(ITaskManager& taskManager, Database& db)
    : mTaskManager(taskManager)
{
    mCfg.environmentSize.x = db.loadParameter<std::uint16_t>("env_size_x");
    mCfg.environmentSize.y = db.loadParameter<std::uint16_t>("env_size_y");
    mCfg.numThreads = db.loadParameter<std::size_t>("num_threads");
    mCfg.populationSize = db.loadParameter<std::size_t>("population_size");
    mCfg.survivalRate = db.loadParameter<double>("survival_rate");
    mGeneration = db.loadParameter<std::size_t>("generation");
    mCurrentEnergyLimit = db.loadParameter<std::size_t>("current_energy_limit");
    mGlobalTaskScores = db.loadScores();
    mPopulation = db.loadPops();
}

void Sori::reconfigure(const Config& cfg)
{
    mCfg = cfg;
}

void Sori::setStatistics(IStatistics& stats)
{
    mStats = stats;
}

void Sori::checkpoint(Database& db)
{
    db.saveParameter("env_size_x", mCfg.environmentSize.x);
    db.saveParameter("env_size_y", mCfg.environmentSize.y);
    db.saveParameter("num_threads", mCfg.numThreads);
    db.saveParameter("population_size", mCfg.populationSize);
    db.saveParameter("survival_rate", mCfg.survivalRate);
    db.saveParameter("generation", mGeneration);
    db.saveParameter("current_energy_limit", mCurrentEnergyLimit);

    db.saveScores(mGlobalTaskScores);
    db.savePops(mPopulation);
}

Config Sori::getConfig() const
{
    return mCfg;
}

std::size_t Sori::getGeneration() const
{
    return mGeneration;
}

int Sori::getLastTaskScore(const std::string& taskName) const
{
    auto pos = mGlobalTaskScores.find(taskName);
    if(pos != mGlobalTaskScores.end())
    {
        return pos->second;
    }

    throw std::runtime_error("No such task " + taskName);
}

const std::list<Pop>& Sori::getPopulation() const
{
    return mPopulation;
}

void Sori::step()
{
   select();
   populate();
   evaluate();
   mGeneration++;
}
void Sori::select()
{
   //Expected to be sorted after evaluation
   std::list<Pop> selected;

   for(std::size_t i = 0; i < std::max(1, static_cast<int>(mCfg.populationSize * mCfg.survivalRate)); ++i)
   {
       selected.splice(selected.end(), mPopulation, selectTournament(mPopulation));
   }

   mPopulation = selected;
}

void Sori::populate()
{
   std::vector<std::list<Pop>::iterator> sources;
   for(auto iter = mPopulation.begin(); iter != mPopulation.end(); ++iter)
   {
       sources.push_back(iter);
   }

   while (mPopulation.size() < mCfg.populationSize)
   {
       //Each of the selected individuums pass equal amount of offspring
       //even if it means go above pop limit
       for(const auto& s : sources)
       {
           mPopulation.push_back(s->cloneMutated());
       }
   }
}

void Sori::evaluate()
{
   auto& task = mTaskManager.pickNextTask(mGlobalTaskScores);

   std::vector<std::thread> threads;
   for(std::size_t i = 0; i < mCfg.numThreads; ++i)
   {
       threads.push_back(std::thread([this, i, &task](){
           Environment ev(mCfg.environmentSize, task, mCurrentEnergyLimit);
           auto iter = mPopulation.begin();
           for(std::size_t popidx = 0; popidx < mPopulation.size(); ++popidx, ++iter)
           {
               if((popidx % mCfg.numThreads) == i)
               {
                   if(mCfg.testMode && mCfg.numThreads == 1)
                   {
                       savePop("LastRunPop", *iter);
                   }
                   ev.run(*iter);
               }
           }
       }));
   }

   for(auto& t : threads)
   {
       t.join();
   }

   mPopulation.sort([](auto& x, auto& y){return x.getFitness() > y.getFitness();});
   const auto maxScore = mPopulation.begin()->getFitness().score;
   if(mStats)
   {
       auto totalScore = std::accumulate(mPopulation.begin(), mPopulation.end(), 0, [](const int score, const auto& pop){return score + pop.getFitness().score;});
       (*mStats).get().onStepResult(task.getName(), mGeneration, mCurrentEnergyLimit, maxScore, totalScore / mPopulation.size());
   }
   if(maxScore < task.getMaxScore())
   {
       mCurrentEnergyLimit += 10;
   }
   mGlobalTaskScores[task.getName()] = maxScore;
}

std::list<Pop>::iterator Sori::selectTournament(std::list<Pop>& pops)
{
    const int NUM_PARTICIPANTS = 8;
    const double PARTICIPANT_CHANCE = 0.5;

    std::vector<std::pair<std::list<Pop>::iterator, sori::Fitness>> candidates;
    for(int i = 0; i < NUM_PARTICIPANTS; ++i)
    {
        auto pos = Rng::genChoise(pops.size());
        auto candidate = pops.begin();
        std::advance(candidate, pos);
        candidates.push_back({candidate, candidate->getFitness()});
    }

    std::sort(candidates.begin(), candidates.end(), [](auto x, auto y){return x.second > y.second;});

    while(true)
    {
        for(int i = 0; i < NUM_PARTICIPANTS; ++i)
        {
            if(Rng::genProbability(PARTICIPANT_CHANCE))
            {
                return candidates[i].first;
            }
        }
    }
}

const Pop& Sori::getTopPerformer() const
{
    return *mPopulation.begin();
}

std::size_t Sori::getEnergyLimit() const
{
    return mCurrentEnergyLimit;
}

}
