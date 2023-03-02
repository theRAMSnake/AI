#include "ObjectDetection.hpp"
#include "gacommon/rng.hpp"
#include "dng/geometry.hpp"
#include "dng/drawing.hpp"

namespace tlib
{

StaticObjectDetection::StaticObjectDetection(const std::vector<std::shared_ptr<dng::Shape>>& shapes, const dng::Color background)
    : mShapes(shapes)
    , mBackground(background)
    , mTagged(mShapes.size())
{
}

bool StaticObjectDetection::isDone() const
{
    return std::find(mTagged.begin(), mTagged.end(), false) == mTagged.end();
}

void StaticObjectDetection::onClick(const dng::Point& pos)
{
    bool clickOnSpot = false;
    for(int i = 0; i < mShapes.size(); ++i)
    {
        if(mShapes[i]->contains(pos))
        {
            clickOnSpot = true;
            mTagged[i] = true;
        }
    }

    if(!clickOnSpot)
    {
        int totalDistance = 0;
        for(auto& s : mShapes)
        {
            totalDistance += dng::distance(pos, s->getCenterPoint());
        }

        mPenalty += totalDistance;
    }
}

int StaticObjectDetection::getScore() const
{
    //Value = 1000 - 250 * untagged_shapes - total_distance_of_missclicks
    return 1000 - std::count(mTagged.begin(), mTagged.end(), false) * 250 - mPenalty;
}

void StaticObjectDetection::draw(dng::Image& surface) const
{
    for(auto& s : mShapes)
    {
        dng::fill(surface, mBackground);
        dng::draw(surface, *s);
    }
}

ObjectDetection1::ObjectDetection1(const dng::Size& envSize)
    : StaticObjectDetection({std::make_shared<dng::Rectangle>(genCoordsToFit(envSize, {10, 10}), dng::Size{10, 10}, dng::colors::Blue)}, dng::colors::Black)
{
}

ObjectDetection2::ObjectDetection2(const dng::Size& envSize)
    : StaticObjectDetection({std::make_shared<dng::Rectangle>(genCoordsToFit(envSize, {10, 10}), dng::Size{10, 10}, dng::genActiveColor())}, dng::colors::Black)
{
}

ObjectDetection3::ObjectDetection3(const dng::Size& envSize)
    : StaticObjectDetection({genShape(envSize)}, dng::colors::Black)
{
}

ObjectDetection4::ObjectDetection4(const dng::Size& envSize)
    : StaticObjectDetection({genShape(envSize)}, dng::genBgColor())
{
}

ObjectDetection5::ObjectDetection5(const dng::Size& envSize)
    : StaticObjectDetection(genShapes(envSize, 2, 5), dng::genBgColor())
{
}
}
