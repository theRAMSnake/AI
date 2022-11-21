#define BOOST_TEST_DYN_LINK
#define TEST

#include <boost/test/unit_test.hpp>
#include "SORI/data.hpp"
#include <algorithm>

class SoriDataTest
{
protected:
};

BOOST_FIXTURE_TEST_CASE( ReadTest, SoriDataTest )
{
    auto src = std::make_shared<sori::Data>(28);
    //3
    (*src)[8 - 2] = 1;
    (*src)[8 - 1] = 1;
    //4
    (*src)[16 - 3] = 1;
    //2
    (*src)[24 - 2] = 1;

    sori::DataReader<std::uint8_t> reader(*src);
    std::vector<std::uint8_t> values;
    std::copy(reader.begin(), reader.end(), std::back_inserter(values));

    BOOST_CHECK_EQUAL(3, values.size());
    BOOST_CHECK_EQUAL(3, values[0]);
    BOOST_CHECK_EQUAL(4, values[1]);
    BOOST_CHECK_EQUAL(2, values[2]);
}

BOOST_FIXTURE_TEST_CASE( ReadCappedTest, SoriDataTest )
{
    auto src = std::make_shared<sori::Data>(1);
    (*src)[0] = 1;

    sori::DataReader<std::uint8_t, 2> reader(*src);
    std::vector<std::uint8_t> values;
    std::copy(reader.begin(), reader.end(), std::back_inserter(values));

    BOOST_CHECK_EQUAL(0, values.size());
}

BOOST_FIXTURE_TEST_CASE( ReadTest2, SoriDataTest )
{
    auto src = std::make_shared<sori::Data>(28);
    //3
    (*src)[8 - 2] = 1;
    (*src)[8 - 1] = 1;
    //4
    (*src)[16 - 3] = 1;
    //2
    (*src)[24 - 2] = 1;

    sori::DataReader<std::uint8_t, 3> reader(*src);
    std::vector<std::uint8_t> values;
    std::copy(reader.begin(), reader.end(), std::back_inserter(values));

    BOOST_CHECK_EQUAL(9, values.size());
    BOOST_CHECK_EQUAL(0, values[0]);
    BOOST_CHECK_EQUAL(0, values[1]);
    BOOST_CHECK_EQUAL(6, values[2]);
    BOOST_CHECK_EQUAL(0, values[3]);
    BOOST_CHECK_EQUAL(2, values[4]);
    BOOST_CHECK_EQUAL(0, values[5]);
    BOOST_CHECK_EQUAL(0, values[6]);
    BOOST_CHECK_EQUAL(2, values[7]);
    BOOST_CHECK_EQUAL(0, values[8]);
}

BOOST_FIXTURE_TEST_CASE( WriteTest, SoriDataTest )
{
    auto dst = std::make_shared<sori::Data>();
    sori::DataWritter<std::uint8_t> writter(*dst);

    writter(5);
    writter(12);
    writter(33);
    writter(111);

    sori::DataReader<std::uint8_t> reader(*dst);
    std::vector<std::uint8_t> values;
    std::copy(reader.begin(), reader.end(), std::back_inserter(values));

    BOOST_CHECK_EQUAL(4, values.size());
    BOOST_CHECK_EQUAL(5, values[0]);
    BOOST_CHECK_EQUAL(12, values[1]);
    BOOST_CHECK_EQUAL(33, values[2]);
    BOOST_CHECK_EQUAL(111, values[3]);
}

BOOST_FIXTURE_TEST_CASE( Write2Test, SoriDataTest )
{
    auto dst = std::make_shared<sori::Data>();
    sori::DataWritter<std::uint8_t, 5> writter(*dst);

    writter(5);
    writter(12);
    writter(29);
    writter(2);
    writter(0);

    sori::DataReader<std::uint8_t, 5> reader(*dst);
    std::vector<std::uint8_t> values;
    std::copy(reader.begin(), reader.end(), std::back_inserter(values));

    BOOST_CHECK_EQUAL(5, values.size());
    BOOST_CHECK_EQUAL(5, values[0]);
    BOOST_CHECK_EQUAL(12, values[1]);
    BOOST_CHECK_EQUAL(29, values[2]);
    BOOST_CHECK_EQUAL(2, values[3]);
    BOOST_CHECK_EQUAL(0, values[4]);
}
