#pragma once
#include "IPlayground.hpp"
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ptree_fwd.hpp>
#include <fstream>
#include <algorithm>
#include <thread>
#include "rng.hpp"

namespace gacommon
{

struct IODefinition
{
    std::vector<gacommon::IOElement> inputs;
    std::vector<gacommon::IOElement> outputs;
};

struct Config
{
   std::size_t populationSize;
   std::size_t championsKept;
   double survivalRate;

   std::size_t numThreads;
};

template<class Pop>
class NaturalSelection
{
public:
   NaturalSelection(
      const Config& cfg,
      const IODefinition& io,
      IFitnessEvaluator& fitnessEvaluator
      )
   : mCfg(cfg)
   , mIo(io)
   , mFitnessEvaluator(fitnessEvaluator)
   , mBestFitness(0.0)
   {
       mPopulation.reserve(mCfg.populationSize);
       for(std::size_t i = 0; i < mCfg.populationSize; ++i)
       {
          mPopulation.push_back(Pop::createMinimal(mIo));
       }
   }

   void step()
   {
       auto newGen = select();
       repopulate(newGen);
       evaluate();
       mGeneration++;
   }

   void reconfigure(const Config& cfg)
   {
       mCfg = cfg;
   }

   const std::vector<Pop>& getPopulation() const
   {
       return mPopulation;
   }

   void saveState(const std::string& fileName)
   {
       boost::property_tree::ptree ar;
       ar.put("best_fitness", mBestFitness);
       ar.put("generation", mGeneration);

       boost::property_tree::ptree pops;
       for(const auto& p : mPopulation)
       {
           boost::property_tree::ptree ch;
           p.saveState(ch);

           pops.push_back(std::make_pair("", ch));
       }

       ar.add_child("population", pops);

       boost::property_tree::write_json(fileName, ar);
   }

   void loadState(const std::string& fileName)
   {
       mPopulation.clear();

       boost::property_tree::ptree ar;
       boost::property_tree::read_json(fileName, ar);

       mBestFitness = ar.get<gacommon::Fitness>("best_fitness");
       mGeneration = ar.get<std::size_t>("generation");
       for(const auto& popState : ar.get_child("population"))
       {
           mPopulation.push_back(Pop::loadState(popState.second));
       }
   }

   std::size_t getGenerationNumber() const
   {
       return mGeneration;
   }

private:
   void repopulate(const std::vector<Pop>& selected)
   {
       mPopulation.clear();

       std::copy(selected.begin(), selected.end(), std::back_inserter(mPopulation));
       if(mCfg.populationSize > selected.size())
       {
           if (mBestFitness != 0)
           {
              for (std::size_t i = 0; i < mCfg.championsKept; ++i)
              {
                  mPopulation.push_back(selected[i]);
              }
           }

           while (true)
           {
               //Each of the selected individuums pass equal amount of offspring
               for(auto& s : selected)
               {
                   mPopulation.push_back(s.cloneMutated(mIo));
                   if(mPopulation.size() == mCfg.populationSize)
                   {
                       break;
                   }
               }
               if(mPopulation.size() == mCfg.populationSize)
               {
                   break;
               }
           }
       }
       else
       {
           while(mPopulation.size() > mCfg.populationSize)
           {
               mPopulation.pop_back();
           }
       }
   }

   const Pop& selectTournament(const std::vector<Pop>& pops)
   {
        const int NUM_PARTICIPANTS = 8;
        const double PARTICIPANT_CHANCE = 0.5;

        std::vector<std::pair<std::size_t, gacommon::Fitness>> indices;
        for(int i = 0; i < NUM_PARTICIPANTS; ++i)
        {
            auto pos = Rng::genChoise(pops.size());
            indices.push_back({pos, pops[pos].fitness});
        }

        std::sort(indices.begin(), indices.end(), [](auto x, auto y){return x.second > y.second;});

        while(true)
        {
            for(int i = 0; i < NUM_PARTICIPANTS; ++i)
            {
                if(Rng::genProbability(PARTICIPANT_CHANCE))
                {
                    return pops[indices[i].first];
                }
            }
        }
   }

   void evaluate()
   {
       if(mCfg.numThreads == 1)
       {
            for(auto& pop : mPopulation)
            {
                auto agent = pop.createAgent(mIo);
                pop.fitness = mFitnessEvaluator.evaluate(*agent);
            }
       }
       else
       {
            std::vector<std::thread> threads;
            for(std::size_t i = 0; i < mCfg.numThreads; ++i)
            {
                threads.push_back(std::thread([this, i](){
                    for(std::size_t popidx = 0; popidx < mPopulation.size(); ++popidx)
                    {
                        if(popidx % mCfg.numThreads == i)
                        {
                            auto agent = mPopulation[popidx].createAgent(mIo);
                            mPopulation[popidx].fitness = mFitnessEvaluator.evaluate(*agent);
                        }
                    }
                }));
            }

            for(auto& t : threads)
            {
                t.join();
            }
       }

       std::sort(mPopulation.begin(), mPopulation.end(), [](auto x, auto y){return x.fitness > y.fitness;});
       mBestFitness = mPopulation[0].fitness;
   }

   std::vector<Pop> select()
   {
       //Keep champions, and *survivalRate* selected randomly, weighted by fitness. Never pick same guy
       std::vector<Pop> result;
       result.reserve(mCfg.populationSize);

       if(mBestFitness != 0)//Keep champions
       {
          std::copy(mPopulation.begin(), mPopulation.begin() + mCfg.championsKept, std::back_inserter(result));
       }

       while(result.size() < mCfg.populationSize * mCfg.survivalRate)
       {
          result.push_back(selectTournament(mPopulation));
       }

       return result;
   }

   Config mCfg;
   IODefinition mIo;
   gacommon::IFitnessEvaluator& mFitnessEvaluator;
   std::vector<Pop> mPopulation;

   gacommon::Fitness mBestFitness;
   std::size_t mGeneration = 1;
};

}
