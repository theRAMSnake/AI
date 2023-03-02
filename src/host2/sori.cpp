#include "sori.hpp"
#include "SORI/sori.hpp"
#include "dng/drawing.hpp"
#include "dng/geometry.hpp"
#include "tasks/manager.hpp"
#include "../../sqlite_modern_cpp/hdr/sqlite_modern_cpp.h"

namespace sori
{

template<class T>
T getOrDefault(const std::string& path, const boost::property_tree::ptree& src, const T def)
{
    auto val = src.get_optional<T>(path);
    if(val)
    {
        return *val;
    }
    else
    {
        return def;
    }
}

Config toSoriConfig(const boost::property_tree::ptree& cfg)
{
    Config result;

    result.environmentSize.x = getOrDefault("environmentSize_x", cfg, 100);
    result.environmentSize.y = getOrDefault("environmentSize_y", cfg, 100);
    result.numThreads = getOrDefault("numThreads", cfg, 3);
    result.populationSize = getOrDefault("populationSize", cfg, 100);
    result.survivalRate = getOrDefault("survivalRate", cfg, 0.4);

    return result;
}

boost::property_tree::ptree toPtree(const Config& cfg)
{
    boost::property_tree::ptree result;

    result.put("environmentSize_x", cfg.environmentSize.x);
    result.put("environmentSize_y", cfg.environmentSize.y);
    result.put("numThreads", cfg.numThreads);
    result.put("populationSize", cfg.populationSize);
    result.put("survivalRate", cfg.survivalRate);

    return result;
}

template<class T, class X>
void print (T& stream, const X& entry)
{
    stream << "[Gen: " << entry.genNumber << ", task: '" << entry.taskName << "', energyLimit: " << entry.energyLimit << ", maxScore: " << entry.maxScore << ", avgScore: " << entry.avgScore;
    stream << std::endl;
}

class StatisticsDatabase : public IStatistics
{
public:
    StatisticsDatabase(const std::filesystem::path& dbPath)
        : mDbPath(dbPath)
    {
    }

    StatisticsDatabase()
    {
    }

    void onStepResult(const std::string& taskName, const std::size_t genNumber, const std::size_t energyLimit, const int maxScore, const int avgScore) override
    {
        mNonPersistEntries.push_back({taskName, genNumber, energyLimit, maxScore, avgScore});
    }

    void printLatestStatistics(const int number, std::stringstream& stream)
    {
        int deficit = number - static_cast<int>(mNonPersistEntries.size());
        for(int i = 0; i < mNonPersistEntries.size() && i < number; ++i)
        {
            print(stream, mNonPersistEntries[mNonPersistEntries.size() - (i + 1)]);
        }
        if(deficit > 0 && mDbPath)
        {
            sqlite::database db(*mDbPath);
            db << "SELECT task, gen, energyLimit, maxScore, avgScore FROM stepResults ORDER BY gen DESC LIMIT ?" << deficit >> [&stream](
                    std::string name, std::size_t gen, std::size_t energyLimit, int maxScore, int avgScore
                ) {
                StepResultEntry entry{name, gen, energyLimit, maxScore, avgScore};
                print(stream, entry);
            };
        }
    }

    void persist(const std::filesystem::path& path)
    {
        mDbPath = path;
        sqlite::database db(path);
        db << "CREATE TABLE IF NOT EXISTS stepResults(task text, gen int primary key, energyLimit int, maxScore int, avgScore int)";
        db << "BEGIN";
        auto ins = db << "INSERT INTO stepResults VALUES(?, ?, ?, ?, ?)";
        for(auto& x : mNonPersistEntries)
        {
            ins << x.taskName << x.genNumber << x.energyLimit << x.maxScore << x.avgScore;
            ins++;
        }
        db << "COMMIT";

        mNonPersistEntries.clear();
    }

private:
    struct StepResultEntry
    {
        std::string taskName;
        std::size_t genNumber;
        std::size_t energyLimit;
        int maxScore;
        int avgScore;

    };

    std::vector<StepResultEntry> mNonPersistEntries;
    std::optional<std::filesystem::path> mDbPath;
};

class Project : public IProject
{
public:
    Project(const boost::property_tree::ptree& cfg)
    {
        auto s = toSoriConfig(cfg);
        mTaskManager = std::make_unique<tlib::TaskManager>();
        mImpl = std::make_unique<Sori>(s, *mTaskManager);
        mStats = std::make_unique<StatisticsDatabase>();
        mImpl->setStatistics(*mStats);
        mCfg = cfg;
    }

    Project(const std::filesystem::path& path, const boost::property_tree::ptree& cfg)
    {
        mTaskManager = std::make_unique<tlib::TaskManager>();
        Database db(path.string());
        mImpl = std::make_unique<Sori>(*mTaskManager, db);
        mStats = std::make_unique<StatisticsDatabase>(path);
        mImpl->setStatistics(*mStats);
        mCfg = cfg;
    }

    boost::property_tree::ptree getConfig() const override
    {
        return mCfg;
    }

    void reconfigure(const boost::property_tree::ptree& cfg) override
    {
        auto s = toSoriConfig(cfg);
        mImpl->reconfigure(s);
        mCfg = cfg;
    }

    void step() override
    {
        mImpl->step();
    }

    void saveState(const std::string& fileName) override
    {
        Database db(fileName);
        mImpl->checkpoint(db);
        mStats->persist(fileName);
    }

    std::size_t getGenerationNumber() const override
    {
        return mImpl->getGeneration();
    }

    std::string printRecentStats() const override
    {
        std::stringstream out;
        mStats->printLatestStatistics(10, out);
        return out.str();
    }

private:
    boost::property_tree::ptree mCfg;
    std::unique_ptr<tlib::TaskManager> mTaskManager;
    std::unique_ptr<Sori> mImpl;
    std::unique_ptr<StatisticsDatabase> mStats;
};

std::unique_ptr<IProject> createProject(const boost::property_tree::ptree& config)
{
    return std::make_unique<Project>(config);
}

std::unique_ptr<IProject> loadProject(const std::filesystem::path& path, const boost::property_tree::ptree& config)
{
    return std::make_unique<Project>(path, config);
}

}
