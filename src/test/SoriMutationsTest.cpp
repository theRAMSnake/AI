
#define BOOST_TEST_DYN_LINK
#define TEST
#include <boost/test/unit_test.hpp>
#include <boost/gil/extension/io/bmp/write.hpp>
#include "SORI/pop.hpp"
#include <iostream>
#include "gacommon/rng.hpp"

class TestTaskContext : public sori::TaskContext
{
public:
    bool isDone() const override
    {
        return false;
    }

    void onClick(const sori::Point& pos) const override
    {
        mLastClickPos = pos;
    }

    int getScore() const override
    {
        return 0;
    }

    mutable sori::Point mLastClickPos;
};

class SoriMutationsTest
{
protected:
    sori::Image mSurface {100, 100};
    TestTaskContext mTaskCtx;
};

BOOST_FIXTURE_TEST_CASE( ManyMutationsTest, SoriMutationsTest )
{
    Rng::seed(time(0));
    auto pop = sori::Pop::createMinimal();
    std::size_t eLimit = 25;

    for(int i = 0; i < 10; ++i)
    {
        sori::savePop("test.pop", pop);
        pop.run(eLimit, mSurface, mTaskCtx);
        eLimit += 25;

        pop = pop.cloneMutated();
    }
}
