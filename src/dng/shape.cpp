#include "shape.hpp"
#include "geometry.hpp"
#include "drawing.hpp"
#include "gacommon/rng.hpp"
#include "boost/mpl/vector.hpp"
#include "boost/mpl/for_each.hpp"
#include "boost/geometry.hpp"

namespace dng
{

namespace bg = boost::geometry;
using bgpoint_t = bg::model::point<double, 2, bg::cs::cartesian>;
using polygon_t = bg::model::polygon<bgpoint_t>;

std::shared_ptr<Shape> genShape(const Size& envSize)
{
    using ShapeTypes = boost::mpl::vector<
        Rectangle,
        Circle,
        Hex,
        Triangle >;

    auto pos = Rng::genChoise(ShapeTypes::size());
    int i = 0;
    std::shared_ptr<Shape> result;

    boost::mpl::for_each<ShapeTypes, boost::mpl::make_identity<boost::mpl::_1>>([&i, envSize, pos, &result](auto arg) {
            if(i++ == pos)
            {
                Size sz {static_cast<uint16_t>(Rng::gen32() % 20 + 5), static_cast<uint16_t>(Rng::gen32() % 20 + 5)};
                result = std::make_shared<typename decltype(arg)::type>(genCoordsToFit(envSize, sz), sz, genActiveColor());
            }
        });

    if(result)
    {
        return result;
    }

    throw std::runtime_error("Logic error in genShape");
}

std::vector<std::shared_ptr<Shape>> genShapes(const Size& envSize, const std::uint16_t min, const std::uint16_t max)
{
    std::vector<std::shared_ptr<Shape>> result;

    auto numItems = Rng::genChoise(max - min) + min;
    for(std::uint16_t i = 0; i < numItems; ++i)
    {
        result.push_back(genShape(envSize));
    }

    return result;
}

Rectangle::Rectangle(const Point& bottomLeft, const Size& sz, const Color color)
    : mBlPos(bottomLeft)
    , mSize(sz)
    , mColor(color)
{
}

bool Rectangle::contains(const Point& pt) const
{
    return ptInRect(pt, mBlPos, mSize);
}

Point Rectangle::getCenterPoint() const
{
    return {static_cast<uint16_t>(mBlPos.x + mSize.x / 2), static_cast<uint16_t>(mBlPos.y + mSize.y / 2)};
}

void Rectangle::project(Image& target) const
{
    fillRect(target, mBlPos, mSize, mColor);
}

Hex::Hex(const Point& bottomLeft, const Size& sz, const Color color)
    : mBlPos(bottomLeft)
    , mSize(sz)
    , mColor(color)
{
}

bool Hex::contains(const Point& pt) const
{
    polygon_t hx {{
        {mBlPos.x, mBlPos.y + mSize.y / 2},
        {mBlPos.x + mSize.x / 3, mBlPos.y + mSize.y},
        {mBlPos.x + (mSize.x / 3) * 2, mBlPos.y + mSize.y},
        {mBlPos.x + mSize.x, mBlPos.y + mSize.y / 2},
        {mBlPos.x + (mSize.x / 3) * 2, mBlPos.y},
        {mBlPos.x + mSize.x / 3, mBlPos.y},
        {mBlPos.x, mBlPos.y + mSize.y / 2}
    }};
    return boost::geometry::within(bgpoint_t{pt.x, pt.y}, hx);
}

Point Hex::getCenterPoint() const
{
    return {static_cast<uint16_t>(mBlPos.x + mSize.x / 2), static_cast<uint16_t>(mBlPos.y + mSize.y / 2)};
}

void Hex::project(Image& target) const
{
    polygon_t hx {{
        {mBlPos.x, mBlPos.y + mSize.y / 2},
        {mBlPos.x + mSize.x / 3, mBlPos.y + mSize.y},
        {mBlPos.x + (mSize.x / 3) * 2, mBlPos.y + mSize.y},
        {mBlPos.x + mSize.x, mBlPos.y + mSize.y / 2},
        {mBlPos.x + (mSize.x / 3) * 2, mBlPos.y},
        {mBlPos.x + mSize.x / 3, mBlPos.y},
        {mBlPos.x, mBlPos.y + mSize.y / 2}
    }};

    auto f = [hx, this] (auto x) {
    };
}
}
