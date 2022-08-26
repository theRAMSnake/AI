#include "Engines.hpp"
#include "neat/neat.hpp"
#include "snake4/snake4.hpp"
#include <boost/filesystem.hpp>
#include <numeric>

unsigned int getRawNumIOs(const std::vector<gacommon::IOElement>& inputs)
{
   unsigned int result = 0;
   for(auto& i : inputs)
   {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr(std::is_same_v<gacommon::ValueIO, T>)
            {
                result++;
            }
            else if constexpr(std::is_same_v<gacommon::ChoiceIO, T>)
            {
                result += arg.options;
            }
            else if constexpr(std::is_same_v<gacommon::BitmapIO, T>)
            {
                result += arg.map.size();
            }
            else
            {
                throw std::runtime_error("Unhandled in run");
            }
        }, i);
   }

   return result;
}

neat::Config makeNeatConfig(const boost::property_tree::ptree& orig, gacommon::IPlayground& pg)
{
    neat::Config result;

    result.numInputs = getRawNumIOs(pg.getInputs());
    result.numOutputs = getRawNumIOs(pg.getOutputs());
    result.numThreads = orig.get<unsigned int>("num_threads");

    result.populationCfg.size = orig.get<unsigned int>("population");
    result.populationCfg.mCompatibilityFactor = orig.get<double>("neat_compatibility_factor", 5.0);
    result.populationCfg.minterspecieCrossoverPercentage = orig.get<double>("neat_interspecie_crossover_percentage", 1);
    result.populationCfg.mC1_C2 = orig.get<double>("neat_c1_c2", 1);
    result.populationCfg.mC3 = orig.get<double>("neat_c3", 0.3);

    result.mutationCfg.perturbationChance = orig.get<double>("neat_perturbation_chance", 0.9);
    result.mutationCfg.addNodeMutationChance = orig.get<double>("neat_add_node_chance", 0.05);
    result.mutationCfg.changeNodeMutationChance = orig.get<double>("neat_change_node_chance", 0.2);
    result.mutationCfg.addConnectionMutationChance = orig.get<double>("neat_add_connection_chance", 0.1);
    result.mutationCfg.removeConnectionMutationChance = orig.get<double>("neat_remove_connection_chance", 0.1);
    result.mutationCfg.removeNodeMutationChance = orig.get<double>("neat_remove_node_chance", 0.05);
    result.mutationCfg.weightsMutationChance = orig.get<double>("neat_weights_mutation_chance", 0.8);

    return result;
}

class NeatProject : public IProject
{
public:
    NeatProject(std::unique_ptr<gacommon::IPlayground>&& playground, const boost::property_tree::ptree cfg)
        : mPlayground(std::move(playground))
        , mCfg(cfg)
        , mImpl(makeNeatConfig(mCfg, *mPlayground), neat::EvolutionStrategyType::Blend, mPlayground->getFitnessEvaluator())
    {
    }

    NeatProject(std::unique_ptr<gacommon::IPlayground>&& playground, const boost::property_tree::ptree cfg, const std::string& filename)
        : mPlayground(std::move(playground))
        , mCfg(cfg)
        , mImpl(makeNeatConfig(mCfg, *mPlayground), neat::EvolutionStrategyType::Blend, mPlayground->getFitnessEvaluator())
    {
        auto phasingEnabled = mCfg.get_optional<bool>("enable_phasing");
        if(phasingEnabled && *phasingEnabled)
        {
            mImpl.reconfigure(makeNeatConfig(mCfg, *mPlayground), neat::EvolutionStrategyType::Phasing);
        }
        mImpl.loadState(filename);
    }

    boost::property_tree::ptree getConfig() const override
    {
        return mCfg;
    }

    void step() override
    {
        mPlayground->step();
        mImpl.step();
    }

    gacommon::Fitness getBestFitness() const override
    {
        return mImpl.getPopulation()[0].maxFitness;
    }

    std::size_t getGenerationNumber() const override
    {
        return mImpl.getGenerationNumber();
    }

    std::vector<PopInfo> describePopulation() const override
    {
        std::vector<PopInfo> result;

        result.reserve(mImpl.getPopulation().size());

        for(const auto& s : mImpl.getPopulation())
        {
            unsigned int i = 0;
            for(auto& p : s.population)
            {
                result.push_back(PopInfo{makePopId(s.id, i++), p.fitness, p.genotype.getComplexity()});
            }
        }

        return result;
    }

    std::vector<std::byte> exportIndividual(const PopId id) const override
    {
        auto tmpFileName = boost::filesystem::unique_path().string();
        std::ofstream tmp(tmpFileName, std::ios::binary | std::ios::trunc);
        auto [specieId, localId] = parsePopId(id);
        for(const auto& s : mImpl.getPopulation())
        {
            if(s.id == specieId && s.population.size() < localId)
            {
                s.population[localId].genotype.write(tmp);

                std::ifstream tmp_load(tmpFileName, std::ios::binary);

                std::vector<std::byte> result;
                std::transform(std::istreambuf_iterator<char>(tmp_load), std::istreambuf_iterator<char>(), std::back_inserter(result), [](auto x){return static_cast<std::byte>(x);});
                return result;
            }
        }

        throw std::runtime_error("Pop not found: " + std::to_string(id));
    }

    std::string perform(const PopId& id) const override
    {
        auto [specieId, localId] = parsePopId(id);
        for(const auto& s : mImpl.getPopulation())
        {
            if(s.id == specieId && s.population.size() > localId)
            {
                gacommon::NNAgent agent(getRawNumIOs(mPlayground->getInputs()) ,getRawNumIOs(mPlayground->getInputs()), mImpl.createAnn(s.population[localId].genotype));
                std::ostringstream out;
                mPlayground->play(agent, out);

                return out.str();
            }
        }
        throw std::runtime_error("Pop not found: " + std::to_string(id));
    }

    void reconfigure(const boost::property_tree::ptree& cfg) override
    {
        auto phasingEnabled = mCfg.get_optional<bool>("enable_phasing");
        if(phasingEnabled && *phasingEnabled)
        {
            mImpl.reconfigure(makeNeatConfig(mCfg, *mPlayground), neat::EvolutionStrategyType::Phasing);
        }
        else
        {
            mImpl.reconfigure(makeNeatConfig(cfg, *mPlayground), neat::EvolutionStrategyType::Blend);
        }
        mCfg = cfg;
    }

    void saveState(const std::string& fileName) override
    {
        mImpl.saveState(fileName);
    }

    double getAverageComplexity() const override
    {
        return mImpl.getPopulation().getAverageComplexity();
    }

private:
    PopId makePopId(const unsigned int specieId, const unsigned int localPopId) const
    {
        return (static_cast<PopId>(specieId) << 32) | localPopId;
    }

    std::tuple<unsigned int, unsigned int> parsePopId(const PopId id) const
    {
        return {id >> 32, id & 0xFFFFFFFF};
    }

    std::unique_ptr<gacommon::IPlayground> mPlayground;
    boost::property_tree::ptree mCfg;
    neat::Neat mImpl;
};

gacommon::Config makeS4Config(const boost::property_tree::ptree& cfg)
{
    gacommon::Config result;

    result.numThreads = cfg.get<unsigned int>("num_threads");
    result.populationSize = cfg.get<unsigned int>("population");

    result.championsKept = cfg.get<unsigned int>("gacommon_champions_kept", 2);
    result.survivalRate = cfg.get<double>("gacommon_survival_rate", 0.25);

    return result;
}

class Snake4Project : public IProject
{
public:
    Snake4Project(std::unique_ptr<gacommon::IPlayground>&& playground, const boost::property_tree::ptree cfg)
        : mPlayground(std::move(playground))
        , mCfg(cfg)
        , mIO{mPlayground->getInputs(), mPlayground->getOutputs()}
        , mImpl(makeS4Config(mCfg), mIO, mPlayground->getFitnessEvaluator())
    {
    }

    Snake4Project(std::unique_ptr<gacommon::IPlayground>&& playground, const boost::property_tree::ptree cfg, const std::string& filename)
        : mPlayground(std::move(playground))
        , mCfg(cfg)
        , mIO{mPlayground->getInputs(), mPlayground->getOutputs()}
        , mImpl(makeS4Config(mCfg), mIO, mPlayground->getFitnessEvaluator())
    {
        mImpl.loadState(filename);
    }

    boost::property_tree::ptree getConfig() const override
    {
        return mCfg;
    }

     void step() override
     {
         mPlayground->step();
         mImpl.step();
     }

    gacommon::Fitness getBestFitness() const override
    {
        return mImpl.getPopulation()[0].fitness;
    }

    std::size_t getGenerationNumber() const override
    {
        return mImpl.getGenerationNumber();
    }

    std::vector<PopInfo> describePopulation() const override
    {
        std::vector<PopInfo> result;

        result.reserve(mImpl.getPopulation().size());

        unsigned int i = 0;
        for(auto& p : mImpl.getPopulation())
        {
            result.push_back(PopInfo{i++, p.fitness, 0});
        }

        return result;
    }

    std::vector<std::byte> exportIndividual(const PopId id) const override
    {
        if(id < mImpl.getPopulation().size())
        {
            boost::property_tree::ptree tree;
            mImpl.getPopulation()[id].saveState(tree);
            std::stringstream str;
            boost::property_tree::write_json(str, tree);

            std::vector<std::byte> result;
            auto buf = str.str();
            std::transform(buf.begin(), buf.end(), std::back_inserter(result), [](auto x){return static_cast<std::byte>(x);});
            return result;
        }

        throw std::runtime_error("Pop not found: " + std::to_string(id));
    }

    std::string perform(const PopId& id) const override
    {
        if(id < mImpl.getPopulation().size())
        {
            auto agent = mImpl.getPopulation()[id].createAgent(mIO);
            std::ostringstream out;
            mPlayground->play(*agent, out);

            return out.str();
        }
        throw std::runtime_error("Pop not found: " + std::to_string(id));
    }

    void reconfigure(const boost::property_tree::ptree& cfg) override
    {
        mImpl.reconfigure(makeS4Config(cfg));
        mCfg = cfg;
    }

    void saveState(const std::string& fileName) override
    {
        mImpl.saveState(fileName);
    }

    double getAverageComplexity() const override
    {
        auto totalComplexity = std::accumulate(mImpl.getPopulation().begin(), mImpl.getPopulation().end(), 0, [](auto x, auto y){return y.blocks.size() + x;});
        return totalComplexity / mImpl.getPopulation().size();
    }

private:
    std::unique_ptr<gacommon::IPlayground> mPlayground;
    boost::property_tree::ptree mCfg;
    gacommon::IODefinition mIO;
    snake4::Algorithm mImpl;
};

std::unique_ptr<IProject> NeatEngine::createProject(std::unique_ptr<gacommon::IPlayground>&& playground, const boost::property_tree::ptree cfg)
{
    return std::make_unique<NeatProject>(std::move(playground), cfg);
}

std::unique_ptr<IProject> NeatEngine::loadProject(std::unique_ptr<gacommon::IPlayground>&& playground, const boost::property_tree::ptree cfg, const std::string& filename)
{
    return std::make_unique<NeatProject>(std::move(playground), cfg, filename);
}

std::unique_ptr<IProject> Snake4Engine::createProject(std::unique_ptr<gacommon::IPlayground>&& playground, const boost::property_tree::ptree cfg)
{
    return std::make_unique<Snake4Project>(std::move(playground), cfg);
}

std::unique_ptr<IProject> Snake4Engine::loadProject(std::unique_ptr<gacommon::IPlayground>&& playground, const boost::property_tree::ptree cfg, const std::string& filename)
{
    return std::make_unique<Snake4Project>(std::move(playground), cfg, filename);
}
