#pragma once
#include <vector>
#include <map>
#include <boost/mpl/vector.hpp>
#include "data.hpp"
#include "task.hpp"

namespace sori
{

using UnitId = std::size_t;

struct Message
{
    std::shared_ptr<Data> data;
    UnitId destination;
};

class Context
{
public:
   Context(const Image& surface, TaskContext& taskCtx);
   bool isDone() const;
   int getErrorRating() const;
   Size getSize() const;

   void onClick(const Point& pt);
   void setProjection(const std::uint64_t key, const Point& pos, const Image& value);
   Image read(const Point& pos, const Size& sz);

private:
   const Image& mSurface;
   std::map<std::uint64_t, std::pair<Point, Image>> mProjections;
   TaskContext& mTaskCtx;
};

class Unit
{
public:
   Unit(const UnitId id_);
   virtual ~Unit(){}

   UnitId getId() const;
   std::size_t getNumConnections() const;

   void connect(const UnitId& destinationId);
   void removeConnection(const std::size_t index);
   void removeConnections(const UnitId outCmpId);
   void postMessage(const Message& msg);
   std::vector<Message> activate(Context& ctx);

   virtual std::shared_ptr<Unit> clone(const UnitId newId) const = 0;
   virtual void mutate() = 0;

protected:
   virtual std::vector<Message> process(Context& ctx, const std::vector<Message>& messages) = 0;

   std::vector<UnitId> mOutputs;
   std::vector<Message> mMessages;
   const UnitId mId;
};

// Manages cursoric connection with environment
class CursorManipulator : public Unit
{
public:
   CursorManipulator(const UnitId id);
   std::shared_ptr<Unit> clone(const UnitId newId) const override;
   void mutate() override;

   static std::shared_ptr<Unit> createRandom(const UnitId id);

private:
   std::vector<Message> process(Context& ctx, const std::vector<Message>& messages) override;
   static Image sProjection;
   Point mPos;
};

// Movable screen sensor
class ScreenReader : public Unit
{
public:
   ScreenReader(const UnitId id, const Size& sz);
   std::shared_ptr<Unit> clone(const UnitId newId) const override;
   void mutate() override;

   static std::shared_ptr<Unit> createRandom(const UnitId id);

private:
   std::vector<Message> process(Context& ctx, const std::vector<Message>& messages) override;
   Point mBottomLeftPos;
   Size mSize;
};

// Generates constant message every frame
class ConstantGenerator : public Unit
{
public:
   ConstantGenerator(const UnitId id);
   std::shared_ptr<Unit> clone(const UnitId newId) const override;
   void mutate() override;

   static std::shared_ptr<Unit> createRandom(const UnitId id);

private:
   std::vector<Message> process(Context& ctx, const std::vector<Message>& messages) override;
   std::shared_ptr<Data> mConstant;
};

// Generates random message with random chance
class RandomGenerator : public Unit
{
public:
   RandomGenerator(const UnitId id);
   std::shared_ptr<Unit> clone(const UnitId newId) const override;
   void mutate() override;

   static std::shared_ptr<Unit> createRandom(const UnitId id);

private:
   std::vector<Message> process(Context& ctx, const std::vector<Message>& messages) override;
   double mChance = 0.0;
   std::size_t mLen = 0;
};

// Generates constant message each X frame
class PhasicGenerator : public Unit
{
public:
   PhasicGenerator(const UnitId id);
   std::shared_ptr<Unit> clone(const UnitId newId) const override;
   void mutate() override;

   static std::shared_ptr<Unit> createRandom(const UnitId id);

private:
   std::vector<Message> process(Context& ctx, const std::vector<Message>& messages) override;
   std::uint8_t mStep = 0;
   std::uint8_t mPhase = 0;
   std::shared_ptr<Data> mConstant;
};

// Stores/Restores data
class Storage : public Unit
{
public:
   Storage(const UnitId id);
   std::shared_ptr<Unit> clone(const UnitId newId) const override;
   void mutate() override;

   static std::shared_ptr<Unit> createRandom(const UnitId id);

private:
   std::vector<Message> process(Context& ctx, const std::vector<Message>& messages) override;
   std::shared_ptr<Data> mSlot;
};

// Extracts data
class Extractor : public Unit
{
public:
   Extractor(const UnitId id, const std::size_t begin, const std::size_t end);
   std::shared_ptr<Unit> clone(const UnitId newId) const override;
   void mutate() override;

   static std::shared_ptr<Unit> createRandom(const UnitId id);

private:
   std::vector<Message> process(Context& ctx, const std::vector<Message>& messages) override;
   std::size_t mBegin = 0;
   std::size_t mEnd = 0;
};

// Combines data
class Combiner : public Unit
{
public:
   Combiner(const UnitId id);
   std::shared_ptr<Unit> clone(const UnitId newId) const override;
   void mutate() override;

   static std::shared_ptr<Unit> createRandom(const UnitId id);
private:
   std::vector<Message> process(Context& ctx, const std::vector<Message>& messages) override;
};

// Applies bitmask on data
class Filter : public Unit
{
public:
   Filter(const UnitId id, const Data& bitmask);
   std::shared_ptr<Unit> clone(const UnitId newId) const override;
   void mutate() override;

   static std::shared_ptr<Unit> createRandom(const UnitId id);

private:
   std::vector<Message> process(Context& ctx, const std::vector<Message>& messages) override;
   Data mBitmask;
};

// Propagates 1 if input1 matches input2 above threshold, else propagates 0
class Matcher : public Unit
{
public:
   Matcher(const UnitId id, const double threshold);
   std::shared_ptr<Unit> clone(const UnitId newId) const override;
   void mutate() override;

   static std::shared_ptr<Unit> createRandom(const UnitId id);

private:
   std::vector<Message> process(Context& ctx, const std::vector<Message>& messages) override;
   double mThreshold = 0.0;
   std::shared_ptr<Data> mSignalTrue;
   std::shared_ptr<Data> mSignalFalse;
};

// Executes logical operation on data.
class LogicalOp : public Unit
{
public:
   enum class Type
   {
       And,
       Or,
       Not,
       Xor
   };

   LogicalOp(const UnitId id, const Type t);
   std::shared_ptr<Unit> clone(const UnitId newId) const override;
   void mutate() override;

   static std::shared_ptr<Unit> createRandom(const UnitId id);
private:
   std::vector<Message> process(Context& ctx, const std::vector<Message>& messages) override;
   Type mType = Type::And;
};

using AllUnitTypes = boost::mpl::vector<
    CursorManipulator,
    ScreenReader,
    ConstantGenerator,
    RandomGenerator,
    PhasicGenerator,
    Storage,
    Extractor,
    Combiner,
    Filter,
    Matcher,
    LogicalOp>;

}
