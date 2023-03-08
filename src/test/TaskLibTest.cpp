
#define TEST
#include <boost/test/unit_test.hpp>
#include "tasks/ObjectDetection.hpp"
#include "dng/shape.hpp"

class TaskLibTest {};

BOOST_FIXTURE_TEST_CASE(StaticObjectDetectionTest1, TaskLibTest)
{
    tlib::StaticObjectDetection sub({std::make_shared<dng::Rectangle>(dng::Point{5, 5}, dng::Size{5, 5}, dng::colors::Blue)},
            dng::colors::Black);

    BOOST_CHECK(!sub.isDone());

    //Click exactly at the shape
    sub.onClick({6, 6});

    BOOST_CHECK(sub.isDone());
    BOOST_CHECK_EQUAL(1000, sub.getScore());
}

BOOST_FIXTURE_TEST_CASE(StaticObjectDetectionTest2, TaskLibTest)
{
    tlib::StaticObjectDetection sub({std::make_shared<dng::Rectangle>(dng::Point{5, 5}, dng::Size{5, 5}, dng::colors::Blue)},
            dng::colors::Black);

    BOOST_CHECK(!sub.isDone());

    sub.onClick({16, 16});

    BOOST_CHECK(!sub.isDone());
    BOOST_CHECK(1000 > sub.getScore());

    sub.onClick({6, 6});

    BOOST_CHECK(sub.isDone());
    BOOST_CHECK(1000 > sub.getScore());
}
BOOST_FIXTURE_TEST_CASE(StaticObjectDetectionTest3, TaskLibTest)
{
    tlib::StaticObjectDetection sub({
            std::make_shared<dng::Rectangle>(dng::Point{5, 5}, dng::Size{5, 5}, dng::colors::Blue),
            std::make_shared<dng::Rectangle>(dng::Point{15, 15}, dng::Size{5, 5}, dng::colors::Blue)
            }, dng::colors::Black);

    BOOST_CHECK(!sub.isDone());

    sub.onClick({6, 6});

    BOOST_CHECK(!sub.isDone());

    sub.onClick({16, 16});

    BOOST_CHECK(sub.isDone());
    BOOST_CHECK_EQUAL(1000, sub.getScore());
}

BOOST_FIXTURE_TEST_CASE(StaticObjectDetectionTest4, TaskLibTest)
{
    tlib::StaticObjectDetection sub({
            std::make_shared<dng::Rectangle>(dng::Point{5, 5}, dng::Size{5, 5}, dng::colors::Blue),
            std::make_shared<dng::Rectangle>(dng::Point{15, 15}, dng::Size{5, 5}, dng::colors::Blue)
            }, dng::colors::Black);

    BOOST_CHECK(!sub.isDone());

    sub.onClick({6, 6});

    BOOST_CHECK(!sub.isDone());

    sub.onClick({26, 26});

    BOOST_CHECK(!sub.isDone());
    BOOST_CHECK(751 > sub.getScore());

    sub.onClick({16, 16});

    BOOST_CHECK(sub.isDone());
    BOOST_CHECK(1000 > sub.getScore());
}
