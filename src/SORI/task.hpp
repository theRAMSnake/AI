#pragma once
#include <string>
#include <boost/gil.hpp>

namespace sori
{

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
};

struct Size
{
    std::uint16_t x = 0;
    std::uint16_t y = 0;
};

using Image = boost::gil::rgb8_image_t;

class TaskContext
{
public:
    virtual bool isDone() const = 0;
    virtual void onClick(const Point& pos) const = 0;
    virtual int getErrorRating() const = 0;
    virtual ~TaskContext() {}
};

class ITask
{
public:
    virtual ~ITask() {}

    virtual std::string getName() const = 0;
    virtual void draw(Image& surface) const = 0;
    virtual std::unique_ptr<TaskContext> createContext() const = 0;
    virtual void advance() = 0;
};

}
