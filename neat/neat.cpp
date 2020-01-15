#include "neat.hpp"
#include "rng.hpp"
#include <ctime>
#include <algorithm>
#include <future>
#include <numeric>
#include <cmath>
#include <fstream>
#include <set>
#include "../logger/Logger.hpp"
#include "neuroevolution/neuro_net.hpp"

namespace neat
{

Neat::Neat(const Config& cfg, const EvolutionStrategyType esType, neuroevolution::IFitnessEvaluator& fitnessEvaluator)
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
    neuroevolution::IFitnessEvaluator* eval, 
    std::vector<std::vector<Pop>::iterator>::iterator begin, 
    std::vector<std::vector<Pop>::iterator>::iterator end
    )
{
    for(auto iter = begin; iter != end; ++iter)
    {
        (*iter)->fitness = eval->evaluate(*createAnn((*iter)->genotype));
    }

    return 0;
}

void Neat::evaluateParallel( std::vector<std::vector<Pop>::iterator>& popPtrs, neuroevolution::IFitnessEvaluator& eval, int numThreads)
{
    std::vector<std::future<int>> fs;
    std::size_t numElementsByThread = popPtrs.size() / numThreads + 1;

    auto first = popPtrs.begin();
    auto last = first + numElementsByThread;
    for(int i = 0; i < numThreads; ++i)
    {
        auto f = std::async(
            std::launch::async, 
            std::bind(&Neat::evaluate, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), 
            &eval, 
            first, 
            last
            );
        fs.push_back(std::move(f));

        first += numElementsByThread;
        last += numElementsByThread;

        if(last > popPtrs.end())
        {
            last = popPtrs.end();
        }
    }
    
    for(auto& f : fs)
    {
        f.get();
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

std::unique_ptr<neuroevolution::NeuroNet> Neat::createAnn(const v2::Genom& src) const
{
    if(mSubstrate)
    {
        return mSubstrate->apply(src);
    }
    else
    {
        return v2::createAnn(src);
    }
}

Neat::Neat(
    const Config& cfg, 
    const neuroevolution::DomainGeometry& domainGeometry, 
    const EvolutionStrategyType esType, 
    neuroevolution::IFitnessEvaluator& fitnessEvaluator
    )
    : Neat(cfg, esType, fitnessEvaluator)
{
    mCfg.numInputs = 6; //(x1, y1, z1, x2, y2, z2)
    mCfg.numOutputs = 1; //weight
    mSubstrate.emplace(domainGeometry);
}

}