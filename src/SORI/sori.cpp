#include "sori.hpp"

namespace sori
{

Sori::Sori(const Config& cfg)
: mCfg(cfg)
{
  for(std::size_t i = 0; i < mCfg.populationSize; ++i)
  {
     mPopulation.push_back(Pop::createMinimal());
  }
}

void Sori::addTask(std::shared_ptr<ITask> task)
{
    mTasks.push_back(task);
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

   for(std::size_t i = 0; i < mCfg.populationSize * mCfg.survivalRate; ++i)
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
   auto task = mTasks[Rng::genChoise(mTasks.size())];
   std::vector<std::uint8_t> results(mPopulation.size());

   std::vector<std::thread> threads;
   for(std::size_t i = 0; i < mCfg.numThreads; ++i)
   {
       threads.push_back(std::thread([this, i, &results, task](){
           Environment ev(mCfg.environmentSize, *task, mCurrentEnergyLimit);
           auto iter = mPopulation.begin();
           for(std::size_t popidx = 0; popidx < mPopulation.size(); ++popidx, ++iter)
           {
               if(popidx % mCfg.numThreads == i)
               {
                   ev.run(*iter);
                   if(iter->getFitness().energyLeft == 0)
                   {
                       results[popidx] = 0;
                   }
                   else
                   {
                       results[popidx] = 1;
                   }
               }
           }
       }));
   }

   for(auto& t : threads)
   {
       t.join();
   }

   std::size_t solutions = std::count(results.begin(), results.end(), 1);
   auto& stats = mTaskStatistics[task->getName()];
   if(stats.size() == 50)
   {
       stats.erase(stats.begin());
   }

   if(solutions == 0)
   {
       mCurrentEnergyLimit += 50;
   }
   else
   {
       task->advance();
   }

   stats.push_back(static_cast<double>(solutions) / mPopulation.size());

   mPopulation.sort([](auto x, auto y){return x.getFitness() > y.getFitness();});
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

}
