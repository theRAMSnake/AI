#pragma once
#include "primitives.hpp"

namespace dng
{

bool ptInRect(const Point& pt, const Point& bottomLeftPos, const Size& sz);
std::uint16_t distance(const Point& a, const Point& b);
Point genCoordsToFit(const Size& area, const Size& size);

}
