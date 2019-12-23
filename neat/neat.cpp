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

namespace neat
{

Neat::Neat(const Config& cfg, const EvolutionStrategyType esType, IFitnessEvaluator& fitnessEvaluator)
: mCfg(cfg)
, mFitnessEvaluator(fitnessEvaluator)
{
    if(esType == EvolutionStrategyType::Blend)
    {
        mEs = std::make_shared<BlendEvolutionStrategy>();
    }
    else if(esType == EvolutionStrategyType::Phasing)
    {
        mEs = std::make_shared<PhasingEvolutionStrategy>();
    }
    
    Rng::seed(static_cast<unsigned int>(std::time(0)));
    Genom::setConfig(cfg);
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
            mCfg.population,
            mCfg.compatibilityFactor,
            mCfg.interspecieCrossoverPercentage,
            mHistory));

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

int evaluate(IFitnessEvaluator* eval, std::vector<std::vector<Pop>::iterator>::iterator begin, std::vector<std::vector<Pop>::iterator>::iterator end)
{
    for(auto iter = begin; iter != end; ++iter)
    {
        (*iter)->fitness = eval->evaluate((*iter)->genotype);
    }

    return 0;
}

void evaluateParallel( std::vector<std::vector<Pop>::iterator>& popPtrs, IFitnessEvaluator& eval, int numThreads)
{
    std::vector<std::future<int>> fs;
    std::size_t numElementsByThread = popPtrs.size() / numThreads + 1;

    auto first = popPtrs.begin();
    auto last = first + numElementsByThread;
    for(int i = 0; i < numThreads; ++i)
    {
        auto f = std::async(std::launch::async, evaluate, &eval, first, last);
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
    if(mCfg.numThreads != 1)
    {
        std::vector<std::vector<Pop>::iterator> popPtrs;
        popPtrs.reserve(mCfg.population * 2); //x2 is not to reallocate overpopulation

        for(auto& s: (*mPopulation))
        {
            for(auto iter = s.population.begin(); iter != s.population.end(); ++iter)
            {
                popPtrs.push_back(iter);
            }
        }

        evaluateParallel(popPtrs, mFitnessEvaluator, mCfg.numThreads);
    }
    else
    {
        for(auto& s: (*mPopulation))
        {
            for(auto iter = s.population.begin(); iter != s.population.end(); ++iter)
            {
                iter->fitness = mFitnessEvaluator.evaluate(iter->genotype);
            }
        }
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
    mPopulation.emplace(mCfg.population, mCfg.compatibilityFactor, mCfg.interspecieCrossoverPercentage);
    mPopulation->setEvolutionStrategy(mEs);
    mPopulation->loadState(ifile, mHistory, mCfg.numInputs, mCfg.numOutputs);
    mHistory.clearCache();
}

void Neat::reconfigure(const Config& cfg, const EvolutionStrategyType esType)
{
    if(esType == EvolutionStrategyType::Blend)
    {
        mEs = std::make_shared<BlendEvolutionStrategy>();
    }
    else if(esType == EvolutionStrategyType::Phasing)
    {
        mEs = std::make_shared<PhasingEvolutionStrategy>();
    }

    auto oldNumInputs = mCfg.numInputs;
    auto oldNumOutputs = mCfg.numOutputs;

    mCfg = cfg;

    mCfg.numInputs = oldNumInputs;
    mCfg.numOutputs = oldNumOutputs;

    Genom::setConfig(mCfg);
    if(mPopulation)
    {
        mPopulation->reconfigure(mCfg.population, mCfg.compatibilityFactor, mCfg.interspecieCrossoverPercentage);
        mPopulation->setEvolutionStrategy(mEs);
    }
}

void Neat::rebase()
{
    //1. Collect all innovations which are enabled in at least one genom
    std::set<InnovationNumber> allInnovations;

    for(auto &s: (*mPopulation))
    {
        for(auto &p: s.population)
        {
            for(auto &g : p.genotype)
            {
                if(g.enabled)
                {
                    allInnovations.insert(g.innovationNumber);
                }
            }
        }
    }

    mHistory = InnovationHistory();

    //2. Rebuild genoms with clean history, skipping outdated innovations
    for(auto &s: (*mPopulation))
    {
        for(auto &p: s.population)
        {
            auto newGenom = Genom(p.genotype.getInputNodeCount(), p.genotype.getOutputNodeCount());

            for(auto &g : p.genotype)
            {
                if(allInnovations.find(g.innovationNumber) != allInnovations.end())
                {
                    newGenom += {g.srcNodeId, g.dstNodeId, g.enabled, mHistory.get(g.srcNodeId, g.dstNodeId), g.weight};
                }
            }

            LOG("Old genom size: " + std::to_string(p.genotype.length()));
            p.genotype = newGenom;
            LOG("New genom size: " + std::to_string(p.genotype.length()));
        }
    }
}

}