#pragma once
#include <string>
#include <boost/gil.hpp>
#include <map>

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

    bool operator<(const Point& other) const
    {
        return x < other.x && y < other.y;
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
    virtual int getScore() const = 0;
    virtual ~TaskContext() {}
};

class ITask
{
public:
    virtual ~ITask() {}

    virtual std::string getName() const = 0;
    virtual void draw(Image& surface) const = 0;
    virtual std::unique_ptr<TaskContext> createContext() const = 0;
    virtual int getMaxScore() const = 0;
};

using TaskScores = std::map<std::string, int>;
class ITaskManager
{
public:
    virtual ~ITaskManager() {}

    virtual ITask& pickNextTask(const TaskScores& taskScores) = 0;
};

}

namespace std
{

template<class stream>
stream& operator << (stream& s, const sori::Point& p)
{
    s << "(" << p.x << ", " << p.y << ")";
    return s;
}

template<class stream>
stream& operator << (stream& s, const sori::Size& p)
{
    s << "(" << p.x << ", " << p.y << ")";
    return s;
}
}
