
#define BOOST_TEST_DYN_LINK
#define TEST
#define BOOST_TEST_MODULE 1
#include <boost/test/unit_test.hpp>
#include <boost/gil/extension/io/bmp/write.hpp>
#include "SORI/components.hpp"

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

class SoriCompsTest
{
protected:
    sori::Image mSurface {100, 100};
    TestTaskContext mTaskCtx;
};

BOOST_FIXTURE_TEST_CASE( BasicTest, SoriCompsTest )
{
    auto unit = sori::ConstantGenerator::createRandom(5);
    sori::Context ctx(mSurface, mTaskCtx);

    BOOST_CHECK_EQUAL(5, unit->getId());

    BOOST_CHECK_EQUAL(0, unit->getNumConnections());

    unit->connect(6);
    unit->connect(7);
    unit->connect(8);
    unit->connect(8);
    unit->connect(9);
    BOOST_CHECK_EQUAL(5, unit->getNumConnections());

    unit->removeConnection(1);
    unit->removeConnections(8);
    BOOST_CHECK_EQUAL(2, unit->getNumConnections());

    sori::Message msg({sori::Data(1), 3});
    unit->postMessage(msg);

    auto result = unit->activate(ctx);

    BOOST_CHECK_EQUAL(2, result.size());
}

template<class T, int Size = sizeof(T) * 8>
class MessageStream
{
public:
    explicit MessageStream(sori::Unit& destination)
        : mDestination(destination)
        , mCurWritter(mCurMsg.data)
    {
    }

    auto& operator << (const T& t)
    {
        mCurWritter(t);
        return *this;
    }

    void flush()
    {
        mDestination.postMessage(mCurMsg);
        mCurMsg.data.clear();
        new (&mCurWritter) sori::DataWritter<T,Size>(mCurMsg.data);
    }

private:
    sori::Unit& mDestination;
    sori::Message mCurMsg;
    sori::DataWritter<T,Size> mCurWritter;
};

BOOST_FIXTURE_TEST_CASE( CursorManipulatorTest, SoriCompsTest )
{
    sori::CursorManipulator unit(0);
    sori::Context ctx(mSurface, mTaskCtx);

    // One message to move x, one to move y, third for click, fourth to move cursor more
    // Expected to have a click at specific position
    MessageStream<std::uint8_t, 3> s(unit);
    s << 0 << 1 << 1 << 1 << 0;
    s.flush();

    s << 2 << 3 << 3 << 3 << 2 << 2;
    s.flush();

    s << 2 << 2 << 4 << 3;
    s.flush();

    s << 1 << 1 << 1;
    s.flush();

    auto result = unit.activate(ctx);
    BOOST_CHECK_EQUAL(0, result.size());

    BOOST_CHECK_EQUAL(1, mTaskCtx.mLastClickPos.x);
    BOOST_CHECK_EQUAL(0, mTaskCtx.mLastClickPos.y);

    for(int i = 0; i < 110; ++i)
    {
        s << 1 << 3;
    }
    s << 4;
    s.flush();

    result = unit.activate(ctx);
    BOOST_CHECK_EQUAL(0, result.size());

    BOOST_CHECK_EQUAL(99, mTaskCtx.mLastClickPos.x);
    BOOST_CHECK_EQUAL(99, mTaskCtx.mLastClickPos.y);
}

BOOST_FIXTURE_TEST_CASE( CursorManipulatorTestDraw, SoriCompsTest )
{
    sori::CursorManipulator unit(0);
    sori::Context ctx(mSurface, mTaskCtx);
    MessageStream<std::uint8_t, 3> s(unit);
    s << 1 << 1 << 1 << 1 << 1;
    s << 3 << 3 << 3 << 3 << 3;
    s.flush();

    auto result = unit.activate(ctx);
    BOOST_CHECK_EQUAL(0, result.size());

    std::vector<sori::Point> expectedPoints;
    expectedPoints.push_back({6, 6});
    expectedPoints.push_back({5, 6});
    expectedPoints.push_back({7, 6});
    expectedPoints.push_back({6, 5});
    expectedPoints.push_back({6, 7});

    sori::Image expected {100, 100};
    boost::gil::rgb8_pixel_t red(255, 0, 0);
    boost::gil::rgb8_pixel_t black(0, 0, 0);

    auto actual = ctx.read({0, 0}, {100, 100});

    auto v = boost::gil::view(actual);
    for(std::uint16_t x = 0; x < 100; x++)
    {
        for(std::uint16_t y = 0; y < 100; ++y)
        {
            if(std::find(expectedPoints.begin(), expectedPoints.end(), sori::Point{x, y}) != expectedPoints.end())
            {
                BOOST_CHECK(red == v(x, y));
            }
            else
            {
                BOOST_CHECK(black == v(x, y));
            }
        }
    }
}

namespace drawing
{

void fillRect(sori::Image& target, const sori::Point& pos, const sori::Size& sz, boost::gil::rgb8_pixel_t color)
{
    auto v = boost::gil::subimage_view(boost::gil::view(target),
                static_cast<int>(pos.x),
                static_cast<int>(pos.y),
                static_cast<int>(sz.x),
                static_cast<int>(sz.y));

    for(auto& p : v)
    {
        p = color;
    }
}

}

BOOST_FIXTURE_TEST_CASE( ScreenReaderTest, SoriCompsTest )
{
    sori::ScreenReader unit(0, {5, 10});
    sori::Context ctx(mSurface, mTaskCtx);

    boost::gil::rgb8_pixel_t blue(0, 0, 255);
    drawing::fillRect(mSurface, {5, 5}, {20, 20}, blue);

    boost::gil::rgb8_pixel_t green(0, 255, 0);
    drawing::fillRect(mSurface, {5, 25}, {20, 20}, green);

    //boost::gil::write_view("test.bmp", boost::gil::view(mSurface), boost::gil::bmp_tag());

    //Move to (4, 20)
    MessageStream<std::uint8_t, 2> s(unit);
    for(int i = 0; i < 4; ++i)
    {
        s << 1;
    }
    for(int i = 0; i < 20; ++i)
    {
        s << 3;
    }
    s.flush();

    unit.connect(6);
    unit.connect(7);

    auto result = unit.activate(ctx);
    BOOST_CHECK_EQUAL(2, result.size());

    //Expect results are the same image
    BOOST_CHECK((result[0].data) == (result[1].data));

    auto& data = (result[0].data);
    BOOST_CHECK_EQUAL(data.size(), 5 * 10 * 3 * 8);

    sori::DataReader<std::uint8_t> reader(data);
    std::vector<std::uint8_t> readData;
    std::copy(reader.begin(), reader.end(), std::back_inserter(readData));
    BOOST_CHECK_EQUAL(readData.size(), 5 * 10 * 3);

    std::vector<std::uint8_t> expected = {
        0, 0, 0, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255,
        0, 0, 0, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255,
        0, 0, 0, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255,
        0, 0, 0, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255,
        0, 0, 0, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255,
        0, 0, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0,
        0, 0, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0,
        0, 0, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0,
        0, 0, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0,
        0, 0, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0
    };

    for(std::size_t i = 0; i < expected.size(); ++i)
    {
        BOOST_CHECK_EQUAL(readData[i], expected[i]);
    }
}

BOOST_FIXTURE_TEST_CASE( ConstantGeneratorTest, SoriCompsTest )
{
    auto unit = sori::ConstantGenerator::createRandom(0);
    sori::Context ctx(mSurface, mTaskCtx);
    unit->connect(6);
    unit->connect(7);
    auto result = unit->activate(ctx);
    BOOST_CHECK_EQUAL(2, result.size());
    BOOST_CHECK((result[0].data) == (result[1].data));

    auto& data = (result[0].data);
    BOOST_CHECK_EQUAL(data.size(), 1);
}

BOOST_FIXTURE_TEST_CASE( RandomGeneratorTest, SoriCompsTest )
{
    auto unit = sori::ConstantGenerator::createRandom(0);
    sori::Context ctx(mSurface, mTaskCtx);
    unit->connect(6);
    unit->connect(7);

    while(true)
    {
        auto result = unit->activate(ctx);
        if(result.size() > 0)
        {
            BOOST_CHECK_EQUAL(2, result.size());
            BOOST_CHECK((result[0].data) == (result[1].data));

            auto& data = (result[0].data);
            BOOST_CHECK_EQUAL(data.size(), 1);

            break;
        }
    }
}

BOOST_FIXTURE_TEST_CASE( PhasicGeneratorTest, SoriCompsTest )
{
    auto unit = sori::PhasicGenerator::createRandom(0);
    sori::Context ctx(mSurface, mTaskCtx);
    unit->connect(6);
    unit->connect(7);

    int numActivations = 0;
    int numPhases = 0;
    while(numActivations < 15)
    {
        auto result = unit->activate(ctx);
        if(result.size() > 0)
        {
            BOOST_CHECK_EQUAL(2, result.size());
            BOOST_CHECK((result[0].data) == (result[1].data));

            auto& data = (result[0].data);
            BOOST_CHECK_EQUAL(data.size(), 1);

            numPhases++;
        }
        numActivations++;
    }
    BOOST_CHECK(numPhases < numActivations);
}

BOOST_FIXTURE_TEST_CASE( StorageTest, SoriCompsTest )
{
    auto unit = sori::Storage::createRandom(0);
    sori::Context ctx(mSurface, mTaskCtx);
    unit->connect(6);
    unit->connect(7);

    MessageStream<std::uint8_t, 1> s(*unit);
    s << 1; //release
    s.flush();

    s << 1 << 1 << 1; //store
    s.flush();

    s << 1; //release
    s.flush();

    s << 0 << 1 << 1; //store
    s.flush();

    s << 1; //release
    s.flush();

    s << 1; //release
    s.flush();

    s << 0; //clear
    s.flush();

    s << 1; //release
    s.flush();

    auto result = unit->activate(ctx);
    BOOST_CHECK_EQUAL(6, result.size());
    BOOST_CHECK((result[0].data) == (result[1].data));
    BOOST_CHECK((result[2].data) == (result[3].data));
    BOOST_CHECK((result[4].data) == (result[5].data));

    {
        auto& data = (result[0].data);
        BOOST_CHECK_EQUAL(data.size(), 3);

        sori::DataReader<std::uint8_t, 1> reader(data);
        std::vector<std::uint8_t> readData;
        std::copy(reader.begin(), reader.end(), std::back_inserter(readData));
        BOOST_CHECK_EQUAL(1, readData[0]);
        BOOST_CHECK_EQUAL(1, readData[1]);
        BOOST_CHECK_EQUAL(1, readData[2]);
    }
    {
        auto& data = (result[2].data);
        BOOST_CHECK_EQUAL(data.size(), 3);

        sori::DataReader<std::uint8_t, 1> reader(data);
        std::vector<std::uint8_t> readData;
        std::copy(reader.begin(), reader.end(), std::back_inserter(readData));
        BOOST_CHECK_EQUAL(0, readData[0]);
        BOOST_CHECK_EQUAL(1, readData[1]);
        BOOST_CHECK_EQUAL(1, readData[2]);
    }
    {
        auto& data = (result[4].data);
        BOOST_CHECK_EQUAL(data.size(), 3);

        sori::DataReader<std::uint8_t, 1> reader(data);
        std::vector<std::uint8_t> readData;
        std::copy(reader.begin(), reader.end(), std::back_inserter(readData));
        BOOST_CHECK_EQUAL(0, readData[0]);
        BOOST_CHECK_EQUAL(1, readData[1]);
        BOOST_CHECK_EQUAL(1, readData[2]);
    }
}
BOOST_FIXTURE_TEST_CASE( ExtractorTest, SoriCompsTest )
{
    sori::Extractor unit(0, 3, 6);
    sori::Context ctx(mSurface, mTaskCtx);
    unit.connect(6);
    unit.connect(7);

    MessageStream<std::uint8_t, 1> s(unit);
    s << 1 << 1;
    s.flush();

    s << 1 << 1 << 1 << 0 << 0;
    s.flush();

    s << 1 << 1 << 1 << 0 << 0 << 0 << 1 << 0;
    s.flush();

    auto result = unit.activate(ctx);
    BOOST_CHECK_EQUAL(6, result.size());
    BOOST_CHECK((result[0].data) == (result[1].data));
    BOOST_CHECK((result[2].data) == (result[3].data));
    BOOST_CHECK((result[4].data) == (result[5].data));
    {
        auto& data = (result[0].data);
        BOOST_CHECK_EQUAL(data.size(), 2);

        sori::DataReader<std::uint8_t, 1> reader(data);
        std::vector<std::uint8_t> readData;
        std::copy(reader.begin(), reader.end(), std::back_inserter(readData));
        BOOST_CHECK_EQUAL(1, readData[0]);
        BOOST_CHECK_EQUAL(1, readData[1]);
    }
    {
        auto& data = (result[2].data);
        BOOST_CHECK_EQUAL(data.size(), 2);

        sori::DataReader<std::uint8_t, 1> reader(data);
        std::vector<std::uint8_t> readData;
        std::copy(reader.begin(), reader.end(), std::back_inserter(readData));
        BOOST_CHECK_EQUAL(0, readData[0]);
        BOOST_CHECK_EQUAL(0, readData[1]);
    }
    {
        auto& data = (result[4].data);
        BOOST_CHECK_EQUAL(data.size(), 3);

        sori::DataReader<std::uint8_t, 1> reader(data);
        std::vector<std::uint8_t> readData;
        std::copy(reader.begin(), reader.end(), std::back_inserter(readData));
        BOOST_CHECK_EQUAL(0, readData[0]);
        BOOST_CHECK_EQUAL(0, readData[1]);
        BOOST_CHECK_EQUAL(0, readData[2]);
    }
}

BOOST_FIXTURE_TEST_CASE( CombinerTest, SoriCompsTest )
{
    sori::Combiner unit(0);
    sori::Context ctx(mSurface, mTaskCtx);
    unit.connect(6);
    unit.connect(7);

    MessageStream<std::uint8_t, 1> s(unit);
    s << 1 << 1;
    s.flush();

    s << 1 << 1 << 1 << 0 << 0;
    s.flush();

    s << 1 << 1 << 1 << 0 << 0 << 0 << 1 << 0;
    s.flush();

    auto result = unit.activate(ctx);
    BOOST_CHECK_EQUAL(2, result.size());
    BOOST_CHECK((result[0].data) == (result[1].data));

    auto& data = (result[0].data);
    BOOST_CHECK_EQUAL(data.size(), 15);

    sori::DataReader<std::uint8_t, 1> reader(data);
    std::vector<std::uint8_t> readData;
    std::copy(reader.begin(), reader.end(), std::back_inserter(readData));

    std::vector<std::uint8_t> expected = {
        1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0
    };

    for(std::size_t i = 0; i < expected.size(); ++i)
    {
        BOOST_CHECK_EQUAL(expected[i], readData[i]);
    }
}

BOOST_FIXTURE_TEST_CASE( FilterTest, SoriCompsTest )
{
    sori::Data d(4);
    d[0] = 1;
    d[1] = 1;
    d[2] = 0;
    d[3] = 0;
    sori::Filter unit(0, d);
    sori::Context ctx(mSurface, mTaskCtx);
    unit.connect(6);
    unit.connect(7);

    MessageStream<std::uint8_t, 1> s(unit);
    s << 1 << 1;
    s.flush();

    s << 1 << 1 << 1 << 0 << 0;
    s.flush();

    s << 1 << 1 << 1 << 0 << 0 << 0 << 1 << 0 << 1;
    s.flush();

    auto result = unit.activate(ctx);
    BOOST_CHECK_EQUAL(6, result.size());
    BOOST_CHECK((result[0].data) == (result[1].data));
    BOOST_CHECK((result[2].data) == (result[3].data));
    BOOST_CHECK((result[4].data) == (result[5].data));
    {
        auto& data = (result[0].data);
        BOOST_CHECK_EQUAL(data.size(), 2);

        sori::DataReader<std::uint8_t, 1> reader(data);
        std::vector<std::uint8_t> readData;
        std::copy(reader.begin(), reader.end(), std::back_inserter(readData));
        BOOST_CHECK_EQUAL(1, readData[0]);
        BOOST_CHECK_EQUAL(1, readData[1]);
    }
    {
        auto& data = (result[2].data);
        BOOST_CHECK_EQUAL(data.size(), 5);

        sori::DataReader<std::uint8_t, 1> reader(data);
        std::vector<std::uint8_t> readData;
        std::copy(reader.begin(), reader.end(), std::back_inserter(readData));
        BOOST_CHECK_EQUAL(1, readData[0]);
        BOOST_CHECK_EQUAL(1, readData[1]);
        BOOST_CHECK_EQUAL(0, readData[2]);
        BOOST_CHECK_EQUAL(0, readData[3]);
        BOOST_CHECK_EQUAL(0, readData[4]);
    }
    {
        auto& data = (result[4].data);
        BOOST_CHECK_EQUAL(data.size(), 9);

        sori::DataReader<std::uint8_t, 1> reader(data);
        std::vector<std::uint8_t> readData;
        std::copy(reader.begin(), reader.end(), std::back_inserter(readData));
        BOOST_CHECK_EQUAL(1, readData[0]);
        BOOST_CHECK_EQUAL(1, readData[1]);
        BOOST_CHECK_EQUAL(0, readData[2]);
        BOOST_CHECK_EQUAL(0, readData[3]);
        BOOST_CHECK_EQUAL(0, readData[4]);
        BOOST_CHECK_EQUAL(0, readData[5]);
        BOOST_CHECK_EQUAL(0, readData[6]);
        BOOST_CHECK_EQUAL(0, readData[7]);
        BOOST_CHECK_EQUAL(1, readData[8]);
    }
}

BOOST_FIXTURE_TEST_CASE( MatcherTest, SoriCompsTest )
{
    sori::Matcher unit(0, 0.7);
    sori::Context ctx(mSurface, mTaskCtx);
    unit.connect(6);
    unit.connect(7);

    MessageStream<std::uint8_t, 1> s(unit);
    s << 1 << 1;
    s.flush();
    s << 0 << 1;
    s.flush();

    s << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1;
    s.flush();
    s << 1 << 1 << 0 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1;
    s.flush();

    s << 1 << 1 << 1 << 0 << 1 << 0 << 1 << 0 << 1 << 1;
    s.flush();
    s << 1 << 1 << 0 << 1 << 1 << 1 << 1 << 1 << 1 << 1 << 1;
    s.flush();

    s << 1 << 1;
    s.flush();
    s << 1 << 1;
    s.flush();

    auto result = unit.activate(ctx);
    BOOST_CHECK_EQUAL(8, result.size());
    BOOST_CHECK((result[0].data) == (result[1].data));
    BOOST_CHECK((result[2].data) == (result[3].data));
    BOOST_CHECK((result[4].data) == (result[5].data));
    BOOST_CHECK((result[6].data) == (result[7].data));
    {
        auto& data = (result[0].data);
        BOOST_CHECK_EQUAL(data.size(), 1);
        BOOST_CHECK(!data[0]);
    }
    {
        auto& data = (result[2].data);
        BOOST_CHECK_EQUAL(data.size(), 1);
        BOOST_CHECK(data[0]);
    }
    {
        auto& data = (result[4].data);
        BOOST_CHECK_EQUAL(data.size(), 1);
        BOOST_CHECK(!data[0]);
    }
    {
        auto& data = (result[6].data);
        BOOST_CHECK_EQUAL(data.size(), 1);
        BOOST_CHECK(data[0]);
    }
}

BOOST_FIXTURE_TEST_CASE( LogicalOpNotTest, SoriCompsTest )
{
    sori::LogicalOp unit(0, sori::LogicalOp::Type::Not);
    sori::Context ctx(mSurface, mTaskCtx);
    unit.connect(6);
    unit.connect(7);

    MessageStream<std::uint8_t, 1> s(unit);
    s << 1 << 1;
    s.flush();
    s << 0 << 1;
    s.flush();

    s << 1 << 1 << 1 << 1 << 1;
    s.flush();
    s << 1 << 1 << 0 << 1 << 1 << 1 << 0 << 0;
    s.flush();

    auto result = unit.activate(ctx);
    BOOST_CHECK_EQUAL(8, result.size());
    BOOST_CHECK((result[0].data) == (result[1].data));
    BOOST_CHECK((result[2].data) == (result[3].data));
    BOOST_CHECK((result[4].data) == (result[5].data));
    BOOST_CHECK((result[6].data) == (result[7].data));
    {
        auto& data = (result[0].data);
        BOOST_CHECK_EQUAL(data.size(), 2);
        sori::DataReader<std::uint8_t, 1> reader(data);
        std::vector<std::uint8_t> readData;
        std::copy(reader.begin(), reader.end(), std::back_inserter(readData));
        BOOST_CHECK_EQUAL(0, readData[0]);
        BOOST_CHECK_EQUAL(0, readData[1]);
    }
    {
        auto& data = (result[2].data);
        BOOST_CHECK_EQUAL(data.size(), 2);
        sori::DataReader<std::uint8_t, 1> reader(data);
        std::vector<std::uint8_t> readData;
        std::copy(reader.begin(), reader.end(), std::back_inserter(readData));
        BOOST_CHECK_EQUAL(1, readData[0]);
        BOOST_CHECK_EQUAL(0, readData[1]);
    }
    {
        auto& data = (result[4].data);
        BOOST_CHECK_EQUAL(data.size(), 5);
        sori::DataReader<std::uint8_t, 1> reader(data);
        std::vector<std::uint8_t> readData;
        std::copy(reader.begin(), reader.end(), std::back_inserter(readData));
        BOOST_CHECK_EQUAL(0, readData[0]);
        BOOST_CHECK_EQUAL(0, readData[1]);
        BOOST_CHECK_EQUAL(0, readData[2]);
        BOOST_CHECK_EQUAL(0, readData[3]);
        BOOST_CHECK_EQUAL(0, readData[4]);
    }
    {
        auto& data = (result[6].data);
        BOOST_CHECK_EQUAL(data.size(), 8);
        sori::DataReader<std::uint8_t, 1> reader(data);
        std::vector<std::uint8_t> readData;
        std::copy(reader.begin(), reader.end(), std::back_inserter(readData));
        BOOST_CHECK_EQUAL(0, readData[0]);
        BOOST_CHECK_EQUAL(0, readData[1]);
        BOOST_CHECK_EQUAL(1, readData[2]);
        BOOST_CHECK_EQUAL(0, readData[3]);
        BOOST_CHECK_EQUAL(0, readData[4]);
        BOOST_CHECK_EQUAL(0, readData[5]);
        BOOST_CHECK_EQUAL(1, readData[6]);
        BOOST_CHECK_EQUAL(1, readData[7]);
    }
}
BOOST_FIXTURE_TEST_CASE( LogicalOpAndTest, SoriCompsTest )
{
    sori::LogicalOp unit(0, sori::LogicalOp::Type::And);
    sori::Context ctx(mSurface, mTaskCtx);
    unit.connect(6);
    unit.connect(7);

    MessageStream<std::uint8_t, 1> s(unit);
    s << 1 << 1;
    s.flush();
    s << 0 << 1;
    s.flush();

    s << 1 << 1 << 1 << 1 << 1;
    s.flush();
    s << 1 << 1 << 0 << 1 << 1 << 1 << 0 << 0;
    s.flush();

    auto result = unit.activate(ctx);
    BOOST_CHECK_EQUAL(4, result.size());
    BOOST_CHECK((result[0].data) == (result[1].data));
    BOOST_CHECK((result[2].data) == (result[3].data));
    {
        auto& data = (result[0].data);
        BOOST_CHECK_EQUAL(data.size(), 2);
        sori::DataReader<std::uint8_t, 1> reader(data);
        std::vector<std::uint8_t> readData;
        std::copy(reader.begin(), reader.end(), std::back_inserter(readData));
        BOOST_CHECK_EQUAL(0, readData[0]);
        BOOST_CHECK_EQUAL(1, readData[1]);
    }
    {
        auto& data = (result[2].data);
        BOOST_CHECK_EQUAL(data.size(), 8);
        sori::DataReader<std::uint8_t, 1> reader(data);
        std::vector<std::uint8_t> readData;
        std::copy(reader.begin(), reader.end(), std::back_inserter(readData));
        BOOST_CHECK_EQUAL(1, readData[0]);
        BOOST_CHECK_EQUAL(1, readData[1]);
        BOOST_CHECK_EQUAL(0, readData[2]);
        BOOST_CHECK_EQUAL(1, readData[3]);
        BOOST_CHECK_EQUAL(1, readData[4]);
        BOOST_CHECK_EQUAL(1, readData[5]);
        BOOST_CHECK_EQUAL(0, readData[6]);
        BOOST_CHECK_EQUAL(0, readData[7]);
    }
}
BOOST_FIXTURE_TEST_CASE( LogicalOpOrTest, SoriCompsTest )
{
    sori::LogicalOp unit(0, sori::LogicalOp::Type::Or);
    sori::Context ctx(mSurface, mTaskCtx);
    unit.connect(6);
    unit.connect(7);

    MessageStream<std::uint8_t, 1> s(unit);
    s << 1 << 1;
    s.flush();
    s << 0 << 1;
    s.flush();

    s << 1 << 1 << 1 << 1 << 0;
    s.flush();
    s << 1 << 1 << 0 << 1 << 0 << 1 << 0 << 0;
    s.flush();

    auto result = unit.activate(ctx);
    BOOST_CHECK_EQUAL(4, result.size());
    BOOST_CHECK((result[0].data) == (result[1].data));
    BOOST_CHECK((result[2].data) == (result[3].data));
    {
        auto& data = (result[0].data);
        BOOST_CHECK_EQUAL(data.size(), 2);
        sori::DataReader<std::uint8_t, 1> reader(data);
        std::vector<std::uint8_t> readData;
        std::copy(reader.begin(), reader.end(), std::back_inserter(readData));
        BOOST_CHECK_EQUAL(1, readData[0]);
        BOOST_CHECK_EQUAL(1, readData[1]);
    }
    {
        auto& data = (result[2].data);
        BOOST_CHECK_EQUAL(data.size(), 8);
        sori::DataReader<std::uint8_t, 1> reader(data);
        std::vector<std::uint8_t> readData;
        std::copy(reader.begin(), reader.end(), std::back_inserter(readData));
        BOOST_CHECK_EQUAL(1, readData[0]);
        BOOST_CHECK_EQUAL(1, readData[1]);
        BOOST_CHECK_EQUAL(1, readData[2]);
        BOOST_CHECK_EQUAL(1, readData[3]);
        BOOST_CHECK_EQUAL(0, readData[4]);
        BOOST_CHECK_EQUAL(1, readData[5]);
        BOOST_CHECK_EQUAL(1, readData[6]);
        BOOST_CHECK_EQUAL(1, readData[7]);
    }
}
BOOST_FIXTURE_TEST_CASE( LogicalOpXorTest, SoriCompsTest )
{
    sori::LogicalOp unit(0, sori::LogicalOp::Type::Xor);
    sori::Context ctx(mSurface, mTaskCtx);
    unit.connect(6);
    unit.connect(7);

    MessageStream<std::uint8_t, 1> s(unit);
    s << 1 << 1;
    s.flush();
    s << 0 << 1;
    s.flush();

    s << 1 << 1 << 1 << 1 << 0;
    s.flush();
    s << 1 << 1 << 0 << 1 << 0 << 1 << 0 << 0;
    s.flush();

    auto result = unit.activate(ctx);
    BOOST_CHECK_EQUAL(4, result.size());
    BOOST_CHECK((result[0].data) == (result[1].data));
    BOOST_CHECK((result[2].data) == (result[3].data));
    {
        auto& data = (result[0].data);
        BOOST_CHECK_EQUAL(data.size(), 2);
        sori::DataReader<std::uint8_t, 1> reader(data);
        std::vector<std::uint8_t> readData;
        std::copy(reader.begin(), reader.end(), std::back_inserter(readData));
        BOOST_CHECK_EQUAL(1, readData[0]);
        BOOST_CHECK_EQUAL(0, readData[1]);
    }
    {
        auto& data = (result[2].data);
        BOOST_CHECK_EQUAL(data.size(), 8);
        sori::DataReader<std::uint8_t, 1> reader(data);
        std::vector<std::uint8_t> readData;
        std::copy(reader.begin(), reader.end(), std::back_inserter(readData));
        BOOST_CHECK_EQUAL(0, readData[0]);
        BOOST_CHECK_EQUAL(0, readData[1]);
        BOOST_CHECK_EQUAL(1, readData[2]);
        BOOST_CHECK_EQUAL(0, readData[3]);
        BOOST_CHECK_EQUAL(0, readData[4]);
        BOOST_CHECK_EQUAL(0, readData[5]);
        BOOST_CHECK_EQUAL(1, readData[6]);
        BOOST_CHECK_EQUAL(1, readData[7]);
    }
}
