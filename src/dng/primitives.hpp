#pragma once
#include <boost/gil.hpp>

namespace dng
{

using Image = boost::gil::rgb8_image_t;

struct Point
{
    std::uint16_t x = 0;
    std::uint16_t y = 0;

    bool operator==(const Point& other) const
    {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Point& other) const
    {
        return !operator==(other);
    }

    bool operator<(const Point& other) const
    {
        return x < other.x && y < other.y;
    }

    Point operator+(const Point& other) const
    {
        return {static_cast<uint16_t>(x + other.x), static_cast<uint16_t>(y + other.y)};
    }
};

struct Size
{
    std::uint16_t x = 0;
    std::uint16_t y = 0;
};

using Color = boost::gil::rgb8_pixel_t;

namespace colors
{
    const Color Blue(0, 0, 255);
    const Color Black(0, 0, 0);
}

}

namespace std
{

template<class stream>
stream& operator << (stream& s, const dng::Point& p)
{
    s << "(" << p.x << ", " << p.y << ")";
    return s;
}

template<class stream>
stream& operator << (stream& s, const dng::Size& p)
{
    s << "(" << p.x << ", " << p.y << ")";
    return s;
}
}
