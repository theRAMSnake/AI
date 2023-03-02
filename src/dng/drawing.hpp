#pragma once
#include "primitives.hpp"
#include "shape.hpp"
#include "boost/gil/algorithm.hpp"

namespace dng
{

void fillRect(Image& target, const Point& pos, const Size& sz, const Color color);
void fill(Image& target, const Color color);
void draw(Image& target, const Shape& shape);
Color genActiveColor();
Color genBgColor();

template<class Functor>
void fillRect(Image& target, const Point& pos, const Size& sz, const Functor f)
{
    auto v = boost::gil::subimage_view(boost::gil::view(target),
                static_cast<int>(pos.x),
                static_cast<int>(pos.y),
                static_cast<int>(sz.x),
                static_cast<int>(sz.y));

    boost::gil::for_each_pixel_position(v, f);
}

}
