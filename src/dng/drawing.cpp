#include "drawing.hpp"
#include "gacommon/rng.hpp"
#include <boost/gil/algorithm.hpp>

namespace dng
{

void fillRect(Image& target, const Point& pos, const Size& sz, const Color color)
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

void fill(Image& target, const Color color)
{
    auto v = boost::gil::view(target);

    for(auto& p : v)
    {
        p = color;
    }
}

void draw(Image& target, const Shape& shape)
{
    shape.project(target);
}

Color genActiveColor()
{
    return Color(Rng::gen32() % 225 + 30, Rng::gen32() % 225 + 30, Rng::gen32() % 225 + 30);
}

Color genBgColor()
{
    return Color(Rng::gen32() % 127, Rng::gen32() % 127, Rng::gen32() % 127);
}

std::vector<bgpoint_t> points_within_polygon(const polygon_t& poly)
{
    std::vector<bgpoint_t> points;
    boost::geometry::model::box<bgpoint_t> b;
    boost::geometry::envelope(poly, b);
    auto min_corner = b.min_corner();
    auto max_corner = b.max_corner();
    for (double x = min_corner.get<0>(); x <= max_corner.get<0>(); ++x) {
        for (double y = min_corner.get<1>(); y <= max_corner.get<1>(); ++y) {
            bgpoint_t p(x, y);
            if (boost::geometry::within(p, poly)) {
                points.push_back(p);
            }
        }
    }
    return points;
}

void fillPolygon(Image& target, const polygon_t& poly, const Color color)
{
    auto points = points_within_polygon(poly);
    auto v = boost::gil::view(target);
    for(auto p : points)
    {
        auto x = p.get<0>();
        auto y = p.get<1>();
        if(x < 0 || x >= v.width() || y < 0 || y >= v.height())
        {
            std::cout << "Trying to draw wrong pixel: " << x << " " << y << std::endl;
            continue;
        }
        v(x, y) = color;
    }
}

}

