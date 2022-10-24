#include "components.hpp"
#include "gacommon/rng.hpp"
#include <string>
#include "data.hpp"
#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>
#include <boost/gil/extension/io/bmp/write.hpp>
#include <iostream>

namespace sori
{

Unit::Unit(const UnitId id_)
    : mId(id_)
{
}

UnitId Unit::getId() const
{
    return mId;
}
void Unit::connect(const UnitId& destinationId)
{
    mOutputs.push_back(destinationId);
}
void Unit::postMessage(const Message& msg)
{
    mMessages.push_back(msg);
}
std::vector<Message> Unit::activate(Context& ctx)
{
    auto result = process(ctx, mMessages);
    mMessages.clear();
    return result;
}
std::size_t Unit::getNumConnections() const
{
    return mOutputs.size();
}
void Unit::removeConnection(const std::size_t index)
{
    mOutputs.erase(mOutputs.begin() + index);
}
Context::Context(const Image& surface, TaskContext& taskCtx)
    : mSurface(surface)
    , mTaskCtx(taskCtx)
{
}
bool Context::isDone() const
{
    return mTaskCtx.isDone();
}
void Unit::removeConnections(const UnitId outCmpId)
{
    std::erase_if(mOutputs, [outCmpId](auto x){return x == outCmpId;});
}
Size Context::getSize() const
{
    return {static_cast<uint16_t>(mSurface.width()), static_cast<uint16_t>(mSurface.height())};
}
int Context::getErrorRating() const
{
    return mTaskCtx.getErrorRating();
}
void Context::onClick(const Point& pt)
{
    mTaskCtx.onClick(pt);
}
void Context::setProjection(const std::uint64_t key, const Point& pt, const Image& value)
{
    mProjections[key] = {pt, value};
}
Image Context::read(const Point& pos, const Size& sz)
{
    Image result(sz.x, sz.y);

    boost::gil::copy_pixels(boost::gil::subimage_view(boost::gil::const_view(mSurface),
                static_cast<int>(pos.x),
                static_cast<int>(pos.y),
                static_cast<int>(sz.x),
                static_cast<int>(sz.y)),
             boost::gil::view(result));

    for(auto& x : mProjections)
    {
        auto& projPos = x.second.first;
        auto projRelativeX = projPos.x - static_cast<int>(pos.x);
        auto projRelativeY = projPos.y - static_cast<int>(pos.y);

        if(projRelativeX < 0 || projRelativeY < 0)
        {
            continue;
        }

        Size projSize{static_cast<std::uint16_t>(x.second.second.width()), static_cast<std::uint16_t>(x.second.second.height())};
        if(projRelativeX + projSize.x > sz.x || projRelativeY + projSize.y > sz.y)
        {
            continue;
        }

        boost::gil::copy_pixels(boost::gil::view(x.second.second), boost::gil::subimage_view(boost::gil::view(result), projRelativeX, projRelativeY, projSize.x, projSize.y));
    }

    return result;
}

template<class T>
void mutateIncDec(T& val)
{
    if(val < 2)
    {
        val++;
    }
    else if(Rng::genProbability(0.5))
    {
        val--;
    }
    else
    {
        val++;
    }
}
//---------------------------------------------------------------------------------------
CursorManipulator::CursorManipulator(const UnitId id)
    : Unit(id)
{
}

std::shared_ptr<Unit> CursorManipulator::clone(const UnitId newId) const
{
    auto result = std::make_shared<CursorManipulator>(newId);
    result->mPos = mPos;
    return result;
}

void CursorManipulator::mutate()
{
}

Point calcPos(const Point& pt, const int xOffset, const int yOffset, const Context& ctx)
{
    int xPos = std::min(std::max(0, pt.x + xOffset), static_cast<int>(ctx.getSize().x - 1));
    int yPos = std::min(std::max(0, pt.y + yOffset), static_cast<int>(ctx.getSize().y - 1));

    return {static_cast<uint16_t>(xPos), static_cast<uint16_t>(yPos)};
}

std::vector<Message> CursorManipulator::process(Context& ctx, const std::vector<Message>& messages)
{
    int xOffset = 0;
    int yOffset = 0;
    for(const auto& m : messages)
    {
        DataReader<std::uint8_t, 3> reader(*m.data);
        for(auto val : reader)
        {
            switch(val)
            {
                case 0:
                    xOffset--;
                    break;
                case 1:
                    xOffset++;
                    break;
                case 2:
                    yOffset--;
                    break;
                case 3:
                    yOffset++;
                    break;
                case 4:
                    mPos = calcPos(mPos, xOffset, yOffset, ctx);
                    ctx.onClick(mPos);
                    break;
                default:
                    ; //Ignore
            }
        }
    }

    mPos = calcPos(mPos, xOffset, yOffset, ctx);
    ctx.setProjection(mId, mPos, sProjection);

    return {};
}

Image createCursorProjection()
{
    boost::gil::rgb8_pixel_t pixel(255, 0, 0);
    Image result(3, 3);
    std::vector<bool> bMask = {false, true, false, true, true, true, false, true, false};
    auto v = boost::gil::view(result);
    auto b = v.begin();
    auto x = bMask.begin();
    while (b != v.end())
    {
        if(*x)
        {
            *b = pixel;
        }
        b++;
        x++;
    }

    return result;
}

Image CursorManipulator::sProjection = createCursorProjection();

std::shared_ptr<Unit> CursorManipulator::createRandom(const UnitId id)
{
    return std::make_shared<CursorManipulator>(id);
}
//---------------------------------------------------------------------------------------
ScreenReader::ScreenReader(const UnitId id, const Size& sz)
    : Unit(id)
    , mSize(sz)
{
}

std::shared_ptr<Unit> ScreenReader::clone(const UnitId newId) const
{
    auto result = std::make_shared<ScreenReader>(newId, mSize);
    result->mBottomLeftPos = mBottomLeftPos;
    return result;
}

void ScreenReader::mutate()
{
    bool isX = Rng::genProbability(0.5);
    if(isX)
    {
        mutateIncDec(mSize.x);
    }
    else
    {
        mutateIncDec(mSize.y);
    }
}

std::vector<Message> ScreenReader::process(Context& ctx, const std::vector<Message>& messages)
{
    int xOffset = 0;
    int yOffset = 0;
    for(const auto& m : messages)
    {
        DataReader<std::uint8_t, 2> reader(*m.data);
        for(auto val : reader)
        {
            switch(val)
            {
                case 0:
                    xOffset--;
                    break;
                case 1:
                    xOffset++;
                    break;
                case 2:
                    yOffset--;
                    break;
                case 3:
                    yOffset++;
                    break;
                default:
                    ; //Ignore
            }
        }
    }

    mBottomLeftPos = calcPos(mBottomLeftPos, xOffset, yOffset, ctx);
    mBottomLeftPos.x = std::min(mBottomLeftPos.x, static_cast<std::uint16_t>(ctx.getSize().x - mSize.x));
    mBottomLeftPos.y = std::min(mBottomLeftPos.y, static_cast<std::uint16_t>(ctx.getSize().y - mSize.y));

    using namespace boost::gil;
    auto img = ctx.read(mBottomLeftPos, mSize);

    struct PixelInserter{
        DataWritter<unsigned char>* writter;
        PixelInserter(DataWritter<unsigned char>* s) : writter(s) {}
        void operator()(boost::gil::rgb8_pixel_t p) const {
                (*writter)(boost::gil::at_c<0>(p));
                (*writter)(boost::gil::at_c<1>(p));
                (*writter)(boost::gil::at_c<2>(p));
        }
    };

    auto out = std::make_shared<Data>();
    DataWritter<unsigned char> writter(*out);
    for_each_pixel(const_view(img), PixelInserter(&writter));

    std::vector<Message> result;
    for(auto d : mOutputs)
    {
        result.push_back({out, d});
    }
    return result;
}

std::shared_ptr<Unit> ScreenReader::createRandom(const UnitId id)
{
    return std::make_shared<ScreenReader>(id, Size{5, 5});
}
//---------------------------------------------------------------------------------------
ConstantGenerator::ConstantGenerator(const UnitId id)
    : Unit(id)
    , mConstant()
{
}

std::shared_ptr<Unit> ConstantGenerator::clone(const UnitId newId) const
{
    auto result = std::make_shared<ConstantGenerator>(newId);
    result->mConstant = cloneData(mConstant);
    return result;
}

void ConstantGenerator::mutate()
{
    bool grow = Rng::genProbability(0.5);
    if(!grow && mConstant->size() > 2)
    {
        mConstant->pop_back();
    }
    else
    {
        mConstant->push_back(Rng::genProbability(0.5) ? 1 : 0);
    }
}

std::vector<Message> ConstantGenerator::process(Context& ctx, const std::vector<Message>& messages)
{
    std::vector<Message> result;
    for(auto d : mOutputs)
    {
        result.push_back({cloneData(mConstant), d});
    }
    return result;
}

std::shared_ptr<Unit> ConstantGenerator::createRandom(const UnitId id)
{
    auto result = std::make_shared<ConstantGenerator>(id);
    result->mConstant = std::make_shared<Data>();
    result->mConstant->push_back(Rng::genProbability(0.5) ? 1 : 0);
    return result;
}
//---------------------------------------------------------------------------------------
RandomGenerator::RandomGenerator(const UnitId id)
    : Unit(id)
{
}

std::shared_ptr<Unit> RandomGenerator::clone(const UnitId newId) const
{
    auto result = std::make_shared<RandomGenerator>(newId);
    result->mChance = mChance;
    result->mLen = mLen;
    return result;
}

void RandomGenerator::mutate()
{
    mChance = Rng::genReal();
    mutateIncDec(mLen);
}

std::vector<Message> RandomGenerator::process(Context& ctx, const std::vector<Message>& messages)
{
    std::vector<Message> result;
    if(Rng::genProbability(mChance))
    {
        auto data = std::make_shared<Data>();
        for(int i = 0; i < mLen; ++i)
        {
            data->push_back(Rng::genProbability(0.5) ? 1 : 0);
        }
        for(auto d : mOutputs)
        {
            result.push_back({data, d});
        }
    }
    return result;
}

std::shared_ptr<Unit> RandomGenerator::createRandom(const UnitId id)
{
    auto result = std::make_shared<RandomGenerator>(id);
    result->mChance = Rng::genReal();
    result->mLen = 1;
    return result;
}
//---------------------------------------------------------------------------------------
PhasicGenerator::PhasicGenerator(const UnitId id)
    : Unit(id)
{
}

std::shared_ptr<Unit> PhasicGenerator::clone(const UnitId newId) const
{
    auto result = std::make_shared<PhasicGenerator>(newId);
    result->mPhase = mPhase;
    result->mStep = mStep;
    result->mConstant = cloneData(mConstant);
    return result;
}

void PhasicGenerator::mutate()
{
    mutateIncDec(mPhase);
    bool grow = Rng::genProbability(0.5);
    if(!grow && mConstant->size() > 2)
    {
        mConstant->pop_back();
    }
    else
    {
        mConstant->push_back(Rng::genProbability(0.5) ? 1 : 0);
    }
}

std::vector<Message> PhasicGenerator::process(Context& ctx, const std::vector<Message>& messages)
{
    std::vector<Message> result;
    mStep++;
    if(mStep >= mPhase)
    {
        mStep = 0;
        for(auto d : mOutputs)
        {
            result.push_back({cloneData(mConstant), d});
        }
    }
    return result;
}

std::shared_ptr<Unit> PhasicGenerator::createRandom(const UnitId id)
{
    auto result = std::make_shared<PhasicGenerator>(id);
    result->mPhase = 2 + Rng::genChoise(5);
    result->mConstant = std::make_shared<Data>();
    result->mConstant->push_back(Rng::genProbability(0.5) ? 1 : 0);
    return result;
}
//---------------------------------------------------------------------------------------
Storage::Storage(const UnitId id)
    : Unit(id)
{
}

std::shared_ptr<Unit> Storage::clone(const UnitId newId) const
{
    auto result = std::make_shared<Storage>(newId);
    result->mSlot = cloneData(mSlot);
    return result;
}

void Storage::mutate()
{
}

std::vector<Message> Storage::process(Context& ctx, const std::vector<Message>& messages)
{
    std::vector<Message> result;
    for(const auto& m : messages)
    {
        //If 1 is recieved - releases, if 0 is recieved - clears
        //Otherwise stores
        if(m.data->size() == 1)
        {
            if((*m.data)[0])
            {
                if(mSlot)
                {
                    for(auto d : mOutputs)
                    {
                        result.push_back({cloneData(mSlot), d});
                    }
                }
            }
            else
            {
                mSlot = {};
            }
        }
        else
        {
            mSlot = cloneData(m.data);
        }
    }

    return result;
}

std::shared_ptr<Unit> Storage::createRandom(const UnitId id)
{
    auto result = std::make_shared<Storage>(id);
    return result;
}
//---------------------------------------------------------------------------------------
Extractor::Extractor(const UnitId id, const std::size_t begin, const std::size_t end)
    : Unit(id)
    , mBegin(begin)
    , mEnd(end)
{
}

std::shared_ptr<Unit> Extractor::clone(const UnitId newId) const
{
    auto result = std::make_shared<Extractor>(newId, mBegin, mEnd);
    return result;
}

void Extractor::mutate()
{
    mutateIncDec(mBegin);
    mutateIncDec(mEnd);

    if(mBegin < mEnd)
    {
        std::swap(mBegin, mEnd);
    }
}

std::vector<Message> Extractor::process(Context& ctx, const std::vector<Message>& messages)
{
    std::vector<Message> result;
    const auto len = mBegin - mEnd;
    for(const auto& m : messages)
    {
        if(m.data->size() < mBegin || len == 0)
        {
            for(auto d : mOutputs)
            {
                result.push_back({cloneData(m.data), d});
            }
            continue;
        }

        auto minEnd = std::min(mEnd, m.data->size());
        auto cutLen = minEnd - mBegin;
        auto cut = std::make_shared<Data>(cutLen);

        for(std::size_t i = 0; i < cutLen; ++i)
        {
            (*cut)[i] = (*m.data)[mBegin + i];
        }

        for(auto d : mOutputs)
        {
            result.push_back({cut, d});
        }
    }

    return result;
}

std::shared_ptr<Unit> Extractor::createRandom(const UnitId id)
{
    auto begin = Rng::genChoise(10);
    auto end = begin + Rng::genChoise(10);
    return std::make_shared<Extractor>(id, begin, end);
}
//---------------------------------------------------------------------------------------
Combiner::Combiner(const UnitId id)
    : Unit(id)
{
}

std::shared_ptr<Unit> Combiner::clone(const UnitId newId) const
{
    auto result = std::make_shared<Combiner>(newId);
    return result;
}

void Combiner::mutate()
{
}

std::vector<Message> Combiner::process(Context& ctx, const std::vector<Message>& messages)
{
    std::vector<Message> result;
    auto combined = std::make_shared<Data>();

    for(std::size_t i = 0; i < messages.size(); i ++)
    {
        auto& a = messages[i];

        for(std::size_t i = 0; i < a.data->size(); ++i)
        {
            combined->push_back((*a.data)[i]);
        }
    }

    for(auto d : mOutputs)
    {
        result.push_back({combined, d});
    }

    return result;
}

std::shared_ptr<Unit> Combiner::createRandom(const UnitId id)
{
    auto result = std::make_shared<Combiner>(id);
    return result;
}
//---------------------------------------------------------------------------------------
Filter::Filter(const UnitId id, const Data& bitmask)
    : Unit(id)
    , mBitmask(bitmask)
{
}

std::shared_ptr<Unit> Filter::clone(const UnitId newId) const
{
    auto result = std::make_shared<Filter>(newId, mBitmask);
    return result;
}

void Filter::mutate()
{
    bool grow = Rng::genProbability(0.5);
    if(!grow && mBitmask.size() > 2)
    {
        mBitmask.pop_back();
    }
    else
    {
        mBitmask.push_back(Rng::genProbability(0.5) ? 1 : 0);
    }
}

std::vector<Message> Filter::process(Context& ctx, const std::vector<Message>& messages)
{
    std::vector<Message> result;
    for(const auto& m : messages)
    {
        auto filtered = cloneData(m.data);

        std::size_t j = 0;
        for(std::size_t i = 0; i < filtered->size(); ++i)
        {
            (*filtered)[i] &= mBitmask[j];
            j++;
            if(j == mBitmask.size())
            {
                j = 0;
            }
        }

        for(auto d : mOutputs)
        {
            result.push_back({filtered, d});
        }
    }

    return result;
}

std::shared_ptr<Unit> Filter::createRandom(const UnitId id)
{
    Data bitmask;
    bitmask.push_back(Rng::genProbability(0.5) ? 1 : 0);
    auto result = std::make_shared<Filter>(id, bitmask);
    return result;
}
//---------------------------------------------------------------------------------------
Matcher::Matcher(const UnitId id, const double threshold)
    : Unit(id)
    , mThreshold(threshold)
{
    mSignalTrue = std::make_shared<Data>(1);
    (*mSignalTrue)[0] = true;
    mSignalFalse = std::make_shared<Data>(1);
    (*mSignalFalse)[0] = false;
}

std::shared_ptr<Unit> Matcher::clone(const UnitId newId) const
{
    auto result = std::make_shared<Matcher>(newId, mThreshold);
    return result;
}

void Matcher::mutate()
{
    mThreshold += Rng::genPerturbation();
}

std::shared_ptr<Unit> Matcher::createRandom(const UnitId id)
{
    auto result = std::make_shared<Matcher>(id, Rng::genReal());
    return result;
}

std::vector<Message> Matcher::process(Context& ctx, const std::vector<Message>& messages)
{
    std::vector<Message> result;
    for(std::size_t i = 0; i < messages.size() - 1; i += 2)
    {
        auto& a = messages[i];
        auto& b = messages[i + 1];

        auto minSize = std::min(a.data->size(), b.data->size());
        auto maxSize = std::max(a.data->size(), b.data->size());

        std::size_t matches = 0;
        for(std::size_t i = 0; i < minSize; ++i)
        {
            if((*a.data)[i] == (*b.data)[i])
            {
                matches++;
            }
        }

        if(static_cast<double>(matches) / maxSize > mThreshold)
        {
            for(auto d : mOutputs)
            {
                result.push_back({mSignalTrue, d});
            }
        }
        else
        {
            for(auto d : mOutputs)
            {
                result.push_back({mSignalFalse, d});
            }
        }
    }

    return result;
}
//---------------------------------------------------------------------------------------
LogicalOp::LogicalOp(const UnitId id, const Type t)
    : Unit(id)
    , mType(t)
{
}

std::shared_ptr<Unit> LogicalOp::clone(const UnitId newId) const
{
    auto result = std::make_shared<LogicalOp>(newId, mType);
    return result;
}

void LogicalOp::mutate()
{
}

std::shared_ptr<Unit> LogicalOp::createRandom(const UnitId id)
{
    auto result = std::make_shared<LogicalOp>(id, static_cast<Type>(Rng::genChoise(4)));
    return result;
}

bool logOp(const bool a, const bool b, LogicalOp::Type type)
{
    if(type == LogicalOp::Type::And)
    {
        return a && b;
    }
    if(type == LogicalOp::Type::Or)
    {
        return a || b;
    }
    if(type == LogicalOp::Type::Xor)
    {
        return a != b;
    }

    return false;
}

std::vector<Message> LogicalOp::process(Context& ctx, const std::vector<Message>& messages)
{
    std::vector<Message> result;
    if(mType != Type::Not)
    {
        for(std::size_t i = 0; i < messages.size() - 1; i += 2)
        {
            auto& a = messages[i];
            auto& b = messages[i + 1];

            auto maxSize = std::max(a.data->size(), b.data->size());
            auto minSize = std::min(a.data->size(), b.data->size());
            if(minSize == 0)
            {
                continue;
            }
            auto combined = std::make_shared<Data>(maxSize);

            std::size_t posa = 0;
            std::size_t posb = 0;
            for(std::size_t i = 0; i < maxSize; ++i)
            {
                (*combined)[i] = logOp((*a.data)[posa], (*b.data)[posb], mType);

                posa++;
                if(posa == a.data->size())
                {
                    posa = 0;
                }

                posb++;
                if(posb == b.data->size())
                {
                    posb = 0;
                }
            }

            for(auto d : mOutputs)
            {
                result.push_back({combined, d});
            }
        }
    }
    else
    {
        for(const auto& m : messages)
        {
            auto cloned = cloneData(m.data);

            cloned->flip();

            for(auto d : mOutputs)
            {
                result.push_back({cloned, d});
            }
        }
    }
    return result;
}
}
