#define BOOST_TEST_DYN_LINK
#define TEST
#include <boost/test/unit_test.hpp>
#include "SORI/sori.hpp"
#include <iostream>
#include <set>
#include "gacommon/rng.hpp"
#include <math.h>

class TestTask1Context : public sori::TaskContext
{
public:
    bool isDone() const override
    {
        return mNumClicked == 5;
    }

    void onClick(const dng::Point& pos) override
    {
        mNumClicked++;
    }

    void draw(dng::Image& surface) const override
    {

    }


    int getScore() const override
    {
        return mNumClicked;
    }

    mutable int mNumClicked = 0;
};

class Test1Task : public sori::ITask
{
public:
    std::string getName() const override
    {
        return "Task1";
    }

    std::unique_ptr<sori::TaskContext> createContext(const dng::Size& envSize) const override
    {
        return std::make_unique<TestTask1Context>();
    }

    int getSolvedScore() const override
    {
        return 5;
    }
};

class TestTaskManager1 : public sori::ITaskManager
{
public:
    sori::ITask& pickNextTask(const sori::TaskScores& scores) override
    {
        return t;
    }

private:
    Test1Task t;
};

class SoriConvergenceTest
{
protected:
    dng::Image mSurface {100, 100};

    void doConvergenceTest(sori::Sori& s, const int maxScore)
    {
        bool solved = false;
        int lastFitness = 0;
        for(int i = 0; i < 10000; ++i)
        {
            s.step();
            auto& pop = s.getTopPerformer();
            if(lastFitness != pop.getFitness().score)
            {
                lastFitness = pop.getFitness().score;
                std::cout << "Now on fitness: " << lastFitness << std::endl;
            }
            if(i % 1000 == 0)
            {
                std::cout << "Generation: " << i << std::endl;
            }
            if(pop.getFitness().score >= maxScore)
            {
                solved = true;
                std::cout << "Converged after " << i << " steps. Energy limit: " << s.getEnergyLimit() << std::endl;

                sori::savePop("Test1Winner.pop", pop);
                break;
            }
        }

        if(!solved)
        {
            BOOST_CHECK(false);
        }
    }
};

BOOST_FIXTURE_TEST_CASE(Test1, SoriConvergenceTest)
{
    Rng::seed(time(0));
    TestTaskManager1 tm;
    sori::Sori s({true, 200, 0.4, 1}, tm);
    doConvergenceTest(s, 5);
}

class TestTask2Context : public sori::TaskContext
{
public:
    bool isDone() const override
    {
        return mPoints.size() == 4;
    }

    void onClick(const dng::Point& pos)override
    {
        mPoints.insert(pos);
    }

    void draw(dng::Image& surface) const override
    {

    }

    int getScore() const override
    {
        return mPoints.size();
    }

    std::set<dng::Point> mPoints;
};

class Test2Task : public sori::ITask
{
public:
    std::string getName() const override
    {
        return "Task2";
    }

    std::unique_ptr<sori::TaskContext> createContext(const dng::Size& envSize) const override
    {
        return std::make_unique<TestTask2Context>();
    }

    int getSolvedScore() const override
    {
        return 4;
    }
};

class TestTaskManager2 : public sori::ITaskManager
{
public:
    sori::ITask& pickNextTask(const sori::TaskScores& scores) override
    {
        return t;
    }

private:
    Test2Task t;
};

BOOST_FIXTURE_TEST_CASE(Test2, SoriConvergenceTest)
{
    Rng::seed(time(0));
    TestTaskManager2 tm;
    sori::Sori s({true, 200, 0.4, 1}, tm);
    doConvergenceTest(s, 4);
}

class TestTask3Context : public sori::TaskContext
{
public:
    bool isDone() const override
    {
        return mDone;
    }

    void onClick(const dng::Point& pos) override
    {
        if(mTarget == pos)
        {
            mDone = true;
            mScore = 1000;
        }
        else
        {
            int x1 = mTarget.x;
            int x2 = pos.x;
            int y1 = mTarget.y;
            int y2 = pos.y;

            mScore = 1000 - static_cast<int>(sqrt(std::pow(x1 - x2, 2) + std::pow(y1 - y2, 2)));
        }
    }

    void draw(dng::Image& surface) const override
    {

    }

    int getScore() const override
    {
        return mScore;
    }

    const dng::Point mTarget {20, 20};
    mutable int mScore = 0;
    mutable bool mDone = false;
};

class Test3Task : public sori::ITask
{
public:
    std::string getName() const override
    {
        return "Task2";
    }

    std::unique_ptr<sori::TaskContext> createContext(const dng::Size& envSize) const override
    {
        return std::make_unique<TestTask3Context>();
    }

    int getSolvedScore() const override
    {
        return 1000;
    }
};

class TestTaskManager3 : public sori::ITaskManager
{
public:
    sori::ITask& pickNextTask(const sori::TaskScores& scores) override
    {
        return t;
    }

private:
    Test3Task t;
};

BOOST_FIXTURE_TEST_CASE(Test3, SoriConvergenceTest)
{
    Rng::seed(time(0));
    TestTaskManager3 tm;
    sori::Sori s({true, 200, 0.4, 1}, tm);
    doConvergenceTest(s, 1000);
}
