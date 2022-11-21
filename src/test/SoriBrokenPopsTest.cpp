
#define BOOST_TEST_DYN_LINK
#define TEST
#include <boost/test/unit_test.hpp>
#include "SORI/components.hpp"
#include "SORI/pop.hpp"
#include "SORI/sori.hpp"
#include <iostream>
#include "gacommon/rng.hpp"

class BrokenPopsContext : public sori::TaskContext
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

class BrokenPopsTest
{
protected:
    sori::Image mSurface {100, 100};
};

BOOST_FIXTURE_TEST_CASE( BrokenPop1Test, BrokenPopsTest )
{
    Rng::seed(time(0));
    auto pop = sori::loadPop("testdata/broken1.pop");

    auto ctx = BrokenPopsContext();
    pop.run(10000, mSurface, ctx);

    for(int i = 0; i < 1000; ++i)
    {
        pop.cloneMutated();
    }
}
BOOST_FIXTURE_TEST_CASE( BrokenPop3Test, BrokenPopsTest )
{
    Rng::seed(time(0));
    auto pop = sori::loadPop("testdata/broken3.pop");

    auto ctx = BrokenPopsContext();
    pop.run(10000, mSurface, ctx);
    for(int i = 0; i < 1000; ++i)
    {
        pop.cloneMutated();
    }
}
