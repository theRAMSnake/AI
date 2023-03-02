#include "geometry.hpp"
#include "gacommon/rng.hpp"

namespace dng
{

bool ptInRect(const Point& pt, const Point& bottomLeftPos, const Size& sz)
{
    return pt.x >= bottomLeftPos.x && pt.x < bottomLeftPos.x + sz.x
        && pt.y >= bottomLeftPos.y && pt.y < bottomLeftPos.y + sz.y;
}

std::uint16_t distance(const Point& a, const Point& b)
{
    int x1 = a.x;
    int x2 = b.x;
    int y1 = a.y;
    int y2 = b.y;
    return static_cast<std::uint16_t>(sqrt(std::pow(x1 - x2, 2) + std::pow(y1 - y2, 2)));
}

Point genCoordsToFit(const Size& area, const Size& size)
{
    return {
        static_cast<uint16_t>(Rng::gen32() % (area.x - size.x)),
        static_cast<uint16_t>(Rng::gen32() % (area.y - size.y))
    };
}

}
