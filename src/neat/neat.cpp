#include "neat.hpp"
#include "gacommon/rng.hpp"
#include <ctime>
#include <algorithm>
#include <future>
#include <numeric>
#include <cmath>
#include <iostream>
#include <fstream>
#include <set>
#include "../logger/Logger.hpp"
#include "gacommon/neuro_net2.hpp"

namespace neat
{

Neat::Neat(const Config& cfg, const EvolutionStrategyType esType, gacommon::IFitnessEvaluator& fitnessEvaluator)
: mCfg(cfg)
, mFitnessEvaluator(fitnessEvaluator)
{
    if(esType == EvolutionStrategyType::Blend)
    {
        mEs = std::make_shared<BlendEvolutionStrategy>(mCfg.mutationCfg);
    }
    else if(esType == EvolutionStrategyType::Phasing)
    {
        mEs = std::make_shared<PhasingEvolutionStrategy>(mCfg.mutationCfg);
    }
    
    Rng::seed(static_cast<unsigned int>(std::time(0)));
}

std::string Neat::getEsInfo() const
{
    return mEs->getInfo();
}

void Neat::step()
{
    if(!mPopulation)
    {
        mPopulation.emplace(Population::createInitialPopulation(
            mCfg.numInputs, 
            mCfg.numOutputs,
            mCfg.populationCfg,
            mHistory
            ));

        mPopulation->setEvolutionStrategy(mEs);
    }
    else
    {
        mPopulation->nextGeneration(mHistory);
    }

    updateFitness();
}

bool Neat::hasPopulation() const
{
    return static_cast<bool>(mPopulation);
}

int Neat::evaluate(
    gacommon::IFitnessEvaluator* eval, 
    std::vector<std::vector<Pop>::iterator>::iterator begin, 
    std::vector<std::vector<Pop>::iterator>::iterator end
    )
{
    for(auto iter = begin; iter != end; ++iter)
    {
        auto agent = gacommon::NNAgent(mCfg.numInputs, mCfg.numOutputs, createAnn((*iter)->genotype));
        (*iter)->fitness = eval->evaluate(agent);
    }

    return 0;
}

void Neat::evaluateParallel( std::vector<std::vector<Pop>::iterator>& popPtrs, gacommon::IFitnessEvaluator& eval, int numThreads)
{
    std::vector<std::thread> threads;
    for(int i = 0; i < numThreads; ++i)
    {
        threads.push_back(std::thread([this, &popPtrs, &eval, numThreads, i](){
                for(int popidx = 0; popidx < static_cast<int>(popPtrs.size()); ++popidx)
                {
                    if(popidx % numThreads == i)
                    {
                        auto agent = gacommon::NNAgent(mCfg.numInputs, mCfg.numOutputs, createAnn(popPtrs[popidx]->genotype));
                        popPtrs[popidx]->fitness = eval.evaluate(agent);
                    }
                }
            }));
    }

    for(auto& t : threads)
    {
        t.join();
    }
}

void Neat::updateFitness()
{
    std::vector<std::vector<Pop>::iterator> popPtrs;
    popPtrs.reserve(mCfg.populationCfg.size * 2); //x2 is not to reallocate overpopulation

    for(auto& s: (*mPopulation))
    {
        for(auto iter = s.population.begin(); iter != s.population.end(); ++iter)
        {
            popPtrs.push_back(iter);
        }
    }

    if(mCfg.numThreads > 1)
    {
        evaluateParallel(popPtrs, mFitnessEvaluator, mCfg.numThreads);
    }
    else
    {
        evaluate(&mFitnessEvaluator, popPtrs.begin(), popPtrs.end());
    }

    mPopulation->onEvaluationFinished();
}

Pop& randomPop(Specie& s)
{
    auto totalFitness = std::accumulate(s.population.begin(), s.population.end(), 0, [](auto a, auto b){return a + b.fitness;});

    auto* p = &s.population[Rng::genChoise(s.population.size())];
    while(!Rng::genProbability((double)p->fitness / totalFitness))
    {
        p = &s.population[Rng::genChoise(s.population.size())];
    }

    return *p;
}

bool comparePopsByFitnessLess(const Pop& a, const Pop& b)
{
   return a.fitness < b.fitness;
}

const Population& Neat::getPopulation() const
{
    return *mPopulation;
}

void Neat::saveState(const std::string& fileName)
{
    std::ofstream ofile(fileName, std::ios::binary | std::ios::trunc);

    mHistory.saveState(ofile);
    if(mPopulation)
    {
        mPopulation->saveState(ofile);
    }
}

void Neat::loadState(const std::string& fileName)
{
    std::ifstream ifile(fileName, std::ios::binary);

    mHistory.loadState(ifile);
    mHistory.buildCache();
    mPopulation.emplace(mCfg.populationCfg);
    mPopulation->setEvolutionStrategy(mEs);
    mPopulation->loadState(ifile, mHistory, mCfg.numInputs, mCfg.numOutputs);
    mHistory.clearCache();
}

void Neat::reconfigure(const Config& cfg, const EvolutionStrategyType esType)
{
    if(esType == EvolutionStrategyType::Blend)
    {
        mEs = std::make_shared<BlendEvolutionStrategy>(cfg.mutationCfg);
    }
    else if(esType == EvolutionStrategyType::Phasing)
    {
        mEs = std::make_shared<PhasingEvolutionStrategy>(cfg.mutationCfg);
    }

    auto oldNumInputs = mCfg.numInputs;
    auto oldNumOutputs = mCfg.numOutputs;

    mCfg = cfg;

    mCfg.numInputs = oldNumInputs;
    mCfg.numOutputs = oldNumOutputs;

    if(mPopulation)
    {
        mPopulation->reconfigure(mCfg.populationCfg);
        mPopulation->setEvolutionStrategy(mEs);
    }
}

std::unique_ptr<gacommon::NeuroNet2> Neat::createAnn(const v2::Genom& src) const
{
    return v2::createAnn2(src);
}

}
