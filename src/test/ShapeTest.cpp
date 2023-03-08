

#define TEST
#include <boost/test/unit_test.hpp>
#include "dng/shape.hpp"

class ShapeTest {};

BOOST_FIXTURE_TEST_CASE(ShapeTest1, ShapeTest)
{
    dng::Rectangle sh({5, 5}, {5, 5}, dng::colors::Blue);
    auto center = sh.getCenterPoint();
    BOOST_CHECK_EQUAL(7, center.x);
    BOOST_CHECK_EQUAL(7, center.y);

    BOOST_CHECK(sh.contains({6, 6}));
    BOOST_CHECK(sh.contains({7, 6}));
    BOOST_CHECK(!sh.contains({7, 60}));
    BOOST_CHECK(!sh.contains({70, 6}));
}
BOOST_FIXTURE_TEST_CASE(ShapeTest2, ShapeTest)
{
    dng::Circle sh({5, 5}, {5, 5}, dng::colors::Blue);
    auto center = sh.getCenterPoint();
    BOOST_CHECK_EQUAL(7, center.x);
    BOOST_CHECK_EQUAL(7, center.y);

    BOOST_CHECK(sh.contains({6, 6}));
    BOOST_CHECK(sh.contains({7, 6}));
    BOOST_CHECK(!sh.contains({7, 60}));
    BOOST_CHECK(!sh.contains({70, 6}));
    BOOST_CHECK(!sh.contains({5, 5}));
}
BOOST_FIXTURE_TEST_CASE(ShapeTest3, ShapeTest)
{
    dng::Hex sh({5, 5}, {5, 5}, dng::colors::Blue);
    auto center = sh.getCenterPoint();
    BOOST_CHECK_EQUAL(7, center.x);
    BOOST_CHECK_EQUAL(7, center.y);

    BOOST_CHECK(sh.contains({6, 6}));
    BOOST_CHECK(sh.contains({7, 6}));
    BOOST_CHECK(!sh.contains({7, 60}));
    BOOST_CHECK(!sh.contains({70, 6}));
    BOOST_CHECK(!sh.contains({5, 5}));
}
BOOST_FIXTURE_TEST_CASE(ShapeTest4, ShapeTest)
{
    dng::Triangle sh({5, 5}, {5, 5}, dng::colors::Blue);
    auto center = sh.getCenterPoint();
    BOOST_CHECK_EQUAL(7, center.x);
    BOOST_CHECK_EQUAL(7, center.y);

    BOOST_CHECK(sh.contains({6, 6}));
    BOOST_CHECK(sh.contains({7, 6}));
    BOOST_CHECK(!sh.contains({7, 60}));
    BOOST_CHECK(!sh.contains({70, 6}));
    BOOST_CHECK(!sh.contains({5, 5}));
}
