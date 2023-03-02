#pragma once
#include "primitives.hpp"

namespace dng
{

class Shape
{
public:
    virtual ~Shape(){}
    virtual bool contains(const Point& pt) const = 0;
    virtual Point getCenterPoint() const = 0;
    virtual void project(Image& target) const = 0;
};

class Rectangle : public Shape
{
public:
    Rectangle(const Point& bottomLeft, const Size& sz, const Color color);

    bool contains(const Point& pt) const override;
    Point getCenterPoint() const override;
    void project(Image& target) const override;

private:
    const Point mBlPos;
    const Size mSize;
    const Color mColor;
};

class Circle : public Shape
{
public:
    Circle(const Point& bottomLeft, const Size& sz, const Color color);

    bool contains(const Point& pt) const override;
    Point getCenterPoint() const override;
    void project(Image& target) const override;

private:
    const Point mBlPos;
    const Size mSize;
    const Color mColor;
};

class Hex : public Shape
{
public:
    Hex(const Point& bottomLeft, const Size& sz, const Color color);

    bool contains(const Point& pt) const override;
    Point getCenterPoint() const override;
    void project(Image& target) const override;

private:
    const Point mBlPos;
    const Size mSize;
    const Color mColor;
};

class Triangle : public Shape
{
public:
    Triangle(const Point& bottomLeft, const Size& sz, const Color color);

    bool contains(const Point& pt) const override;
    Point getCenterPoint() const override;
    void project(Image& target) const override;

private:
    const Point mBlPos;
    const Size mSize;
    const Color mColor;
};

std::shared_ptr<Shape> genShape(const Size& envSize);
std::vector<std::shared_ptr<Shape>> genShapes(const Size& envSize, const std::uint16_t min, const std::uint16_t max);

}
