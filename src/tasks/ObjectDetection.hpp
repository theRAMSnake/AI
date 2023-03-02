#pragma once
#include "Task.hpp"
#include "dng/shape.hpp"

namespace tlib
{

class StaticObjectDetection : public sori::TaskContext
{
public:
    StaticObjectDetection(const std::vector<std::shared_ptr<dng::Shape>>& shapes, const dng::Color background);
    bool isDone() const override;
    void onClick(const dng::Point& pos) override;
    int getScore() const override;
    void draw(dng::Image& surface) const override;

private:
    const std::vector<std::shared_ptr<dng::Shape>> mShapes;
    const dng::Color mBackground;
    std::vector<bool> mTagged;
    int mPenalty = 0;
};

class ObjectDetection1 : public StaticObjectDetection
{
public:
    constexpr static const char* const TASK_NAME = "ObjectDetection1";
    static const int MAX_SCORE = 1000;

    ObjectDetection1(const dng::Size& envSize);
};
class ObjectDetection2 : public StaticObjectDetection
{
public:
    constexpr static const char* const TASK_NAME = "ObjectDetection2";
    static const int MAX_SCORE = 1000;

    ObjectDetection2(const dng::Size& envSize);
};
class ObjectDetection3 : public StaticObjectDetection
{
public:
    constexpr static const char* const TASK_NAME = "ObjectDetection3";
    static const int MAX_SCORE = 1000;

    ObjectDetection3(const dng::Size& envSize);
};
class ObjectDetection4 : public StaticObjectDetection
{
public:
    constexpr static const char* const TASK_NAME = "ObjectDetection4";
    static const int MAX_SCORE = 1000;

    ObjectDetection4(const dng::Size& envSize);
};
class ObjectDetection5 : public StaticObjectDetection
{
public:
    constexpr static const char* const TASK_NAME = "ObjectDetection5";
    static const int MAX_SCORE = 1000;

    ObjectDetection5(const dng::Size& envSize);
};

}
