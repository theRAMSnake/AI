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
                Size sz {static_cast<uint16_t>(Rng::gen32() % 100 + 10), static_cast<uint16_t>(Rng::gen32() % 100 + 10)};
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

polygon_t makeHexPolygon(const Point& pt, const Size& sz)
{
    return polygon_t{{
        {pt.x, pt.y + sz.y / 2},
        {pt.x + sz.x / 3, pt.y + sz.y},
        {pt.x + (sz.x / 3) * 2, pt.y + sz.y},
        {pt.x + sz.x, pt.y + sz.y / 2},
        {pt.x + (sz.x / 3) * 2, pt.y},
        {pt.x + sz.x / 3, pt.y},
        {pt.x, pt.y + sz.y / 2}
    }};
}

bool Hex::contains(const Point& pt) const
{
    polygon_t hx = makeHexPolygon(mBlPos, mSize);
    return boost::geometry::within(bgpoint_t{pt.x, pt.y}, hx);
}

Point Hex::getCenterPoint() const
{
    return {static_cast<uint16_t>(mBlPos.x + mSize.x / 2), static_cast<uint16_t>(mBlPos.y + mSize.y / 2)};
}

void Hex::project(Image& target) const
{
    polygon_t hx = makeHexPolygon(mBlPos, mSize);
    fillPolygon(target, hx, mColor);
}
Triangle::Triangle(const Point& bottomLeft, const Size& sz, const Color color)
    : mBlPos(bottomLeft)
    , mSize(sz)
    , mColor(color)
{
}

polygon_t makeTrianglePolygon(const Point& pt, const Size& sz)
{
    bgpoint_t a = {pt.x, pt.y};
    bgpoint_t b = {pt.x + sz.x / 2, pt.y + sz.y};
    bgpoint_t c = {pt.x + sz.x, pt.y};

    return polygon_t{{
        a, b, c, a
    }};
}

bool Triangle::contains(const Point& pt) const
{
    polygon_t t = makeTrianglePolygon(mBlPos, mSize);
    return boost::geometry::within(bgpoint_t{pt.x, pt.y}, t);
}

Point Triangle::getCenterPoint() const
{
    return {static_cast<uint16_t>(mBlPos.x + mSize.x / 2), static_cast<uint16_t>(mBlPos.y + mSize.y / 2)};
}

void Triangle::project(Image& target) const
{
    polygon_t hx = makeTrianglePolygon(mBlPos, mSize);
    fillPolygon(target, hx, mColor);
}
Circle::Circle(const Point& bottomLeft, const Size& sz, const Color color)
    : mBlPos(bottomLeft)
    , mSize(sz)
    , mColor(color)
{
}

polygon_t makeCirclePolygon(const Point& pt, const Size& sz)
{
    auto minsz = std::min(sz.x, sz.y);
    boost::geometry::strategy::buffer::point_circle point_strategy(360);
    boost::geometry::strategy::buffer::distance_symmetric<double> distance_strategy(minsz / 2);
    boost::geometry::strategy::buffer::join_round join_strategy;
    boost::geometry::strategy::buffer::end_round end_strategy;
    boost::geometry::strategy::buffer::side_straight side_strategy;

    bg::model::multi_polygon<bg::model::polygon<bgpoint_t> > multi;
    boost::geometry::buffer(bgpoint_t{pt.x + sz.x / 2, pt.y + sz.x / 2}, multi,
                distance_strategy, side_strategy,
                join_strategy, end_strategy, point_strategy);

    polygon_t result(multi[0]);

    polygon_t clipArea{{
            {pt.x, pt.y},
            {pt.x, pt.y + sz.x - 1},
            {pt.x + sz.x -1, pt.y + sz.x -1},
            {pt.x + sz.x -1, pt.y},
            {pt.x, pt.y}
        }};

    bg::model::multi_polygon<bg::model::polygon<bgpoint_t> > isec;
    boost::geometry::intersection(result, clipArea, isec);
    return isec[0];
}
bool Circle::contains(const Point& pt) const
{
    polygon_t t = makeCirclePolygon(mBlPos, mSize);
    return boost::geometry::within(bgpoint_t{pt.x, pt.y}, t);
}

Point Circle::getCenterPoint() const
{
    return {static_cast<uint16_t>(mBlPos.x + mSize.x / 2), static_cast<uint16_t>(mBlPos.y + mSize.y / 2)};
}

void Circle::project(Image& target) const
{
    polygon_t hx = makeCirclePolygon(mBlPos, mSize);
    fillPolygon(target, hx, mColor);
}
}
