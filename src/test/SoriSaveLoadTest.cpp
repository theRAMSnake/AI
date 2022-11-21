
#define BOOST_TEST_DYN_LINK
#define TEST
#include <boost/test/unit_test.hpp>
#include "SORI/components.hpp"
#include "SORI/pop.hpp"
#include "SORI/sori.hpp"
#include <iostream>
#include "gacommon/rng.hpp"

class TestTaskSLContext : public sori::TaskContext
{
public:
    bool isDone() const override
    {
        return false;
    }

    void onClick(const sori::Point& pos) const override
    {
    }

    int getScore() const override
    {
        int result = static_cast<int>(Rng::gen32());
        return result;
    }
};

class TestTask : public sori::ITask
{
public:
    std::string getName() const override
    {
        return "Task";
    }

    void draw(sori::Image& surface) const override
    {

    }

    std::unique_ptr<sori::TaskContext> createContext() const override
    {
        return std::make_unique<TestTaskSLContext>();
    }

    int getMaxScore() const override
    {
        return 10000;
    }
};

class TestTaskManager : public sori::ITaskManager
{
public:
    sori::ITask& pickNextTask(const sori::TaskScores& scores) override
    {
        return t;
    }

private:
    TestTask t;
};

class SoriSaveLoadTest
{
protected:
    sori::Image mSurface {100, 100};
};

struct cout_redirect {
    cout_redirect( std::streambuf * new_buffer )
        : old( std::cout.rdbuf( new_buffer ) )
    { }

    ~cout_redirect( ) {
        std::cout.flush();
        std::cout.rdbuf( old );
    }

private:
    std::streambuf * old;
};


BOOST_FIXTURE_TEST_CASE( SaveLoadSimpleTest, SoriSaveLoadTest )
{
    Rng::seed(time(0));
    std::stringstream before;
    std::stringstream after;
    auto pop = sori::Pop::createMinimal();
    {
        cout_redirect(before.rdbuf());
        pop.print();
    }
    sori::savePop("sltest.pop", pop);
    pop = sori::loadPop("sltest.pop");
    {
        cout_redirect(after.rdbuf());
        pop.print();
    }
    BOOST_CHECK_EQUAL(before.str(), after.str());
}

BOOST_FIXTURE_TEST_CASE( SaveLoadTest, SoriSaveLoadTest )
{
    Rng::seed(time(0));
    TestTaskManager tm;
    sori::Sori s({true, 20, 0.4, 1}, tm);
    for(int i = 0; i < 10; ++i)
    {
        s.step();
    }

    sori::Database db("test.db");
    s.checkpoint(db);

    sori::Sori s2(tm, db);

    const auto cfg1 = s.getConfig();
    const auto cfg2 = s2.getConfig();

    BOOST_CHECK(cfg1.environmentSize.x == cfg2.environmentSize.x);
    BOOST_CHECK(cfg1.environmentSize.y == cfg2.environmentSize.y);
    BOOST_CHECK(cfg1.numThreads == cfg2.numThreads);
    BOOST_CHECK(cfg1.populationSize == cfg2.populationSize);
    BOOST_CHECK(cfg1.survivalRate == cfg2.survivalRate);
    BOOST_CHECK_EQUAL(s.getEnergyLimit(), s2.getEnergyLimit());
    BOOST_CHECK_EQUAL(s.getGeneration(), s2.getGeneration());

    auto i2 = s2.getPopulation().begin();

    for(auto i1 = s.getPopulation().begin(); i1 != s.getPopulation().end(); ++i1, ++i2)
    {
        std::stringstream before;
        std::stringstream after;
        {
            cout_redirect(before.rdbuf());
            (*i1).print();
        }
        {
            cout_redirect(after.rdbuf());
            (*i2).print();
        }

        BOOST_CHECK_EQUAL(before.str(), after.str());
    }
    BOOST_CHECK_EQUAL(s.getLastTaskScore("Task"), s2.getLastTaskScore("Task"));
}
