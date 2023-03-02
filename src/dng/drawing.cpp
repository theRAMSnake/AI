#include "drawing.hpp"
#include "gacommon/rng.hpp"

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

}

