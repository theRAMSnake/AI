#pragma once
#include <vector>
#include <iostream>
#include <map>
#include <boost/mpl/vector.hpp>
#include "data.hpp"
#include "task.hpp"
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/dynamic_bitset/serialization.hpp>

namespace sori
{

using UnitId = std::size_t;

struct Message
{
    Data data;
    UnitId destination;
};

class Context
{
public:
   Context(const dng::Image& surface, TaskContext& taskCtx);
   bool isDone() const;
   int getScore() const;
   dng::Size getSize() const;

   void onClick(const dng::Point& pt);
   void setProjection(const std::uint64_t key, const dng::Point& pos, const dng::Image& value);
   dng::Image read(const dng::Point& pos, const dng::Size& sz);

private:
   const dng::Image& mSurface;
   std::map<std::uint64_t, std::pair<dng::Point, dng::Image>> mProjections;
   TaskContext& mTaskCtx;
};

class Unit
{
public:
   friend class boost::serialization::access;
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

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
       ar & mId;
       ar & mOutputs;
   }

   void print() const;

protected:
   Unit(){}
   virtual std::vector<Message> process(Context& ctx, const std::vector<Message>& messages) = 0;
   virtual void printDetails() const = 0;

   std::vector<UnitId> mOutputs;
   std::vector<Message> mMessages;
   UnitId mId = 0;
};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(Unit);

// Manages cursoric connection with environment
class CursorManipulator : public Unit
{
public:
   friend class boost::serialization::access;
   CursorManipulator(const UnitId id);
   std::shared_ptr<Unit> clone(const UnitId newId) const override;
   void mutate() override;

   static std::shared_ptr<Unit> createRandom(const UnitId id);

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
       ar & boost::serialization::base_object<Unit>(*this);
       ar & mPos.x;
       ar & mPos.y;
   }

private:
   CursorManipulator(){}
   std::vector<Message> process(Context& ctx, const std::vector<Message>& messages) override;
   void printDetails() const override;
   static dng::Image sProjection;
   dng::Point mPos;
};

// Movable screen sensor
class ScreenReader : public Unit
{
public:
   friend class boost::serialization::access;
   ScreenReader(const UnitId id, const dng::Size& sz);
   std::shared_ptr<Unit> clone(const UnitId newId) const override;
   void mutate() override;

   static std::shared_ptr<Unit> createRandom(const UnitId id);

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
       ar & boost::serialization::base_object<Unit>(*this);
       ar & mBottomLeftPos.x;
       ar & mBottomLeftPos.y;
       ar & mSize.x;
       ar & mSize.y;
   }

private:
   ScreenReader(){}
   std::vector<Message> process(Context& ctx, const std::vector<Message>& messages) override;
   void printDetails() const override;
   dng::Point mBottomLeftPos;
   dng::Size mSize;
};

// Generates constant message every frame
class ConstantGenerator : public Unit
{
public:
   friend class boost::serialization::access;
   ConstantGenerator(const UnitId id);
   std::shared_ptr<Unit> clone(const UnitId newId) const override;
   void mutate() override;

   static std::shared_ptr<Unit> createRandom(const UnitId id);

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
       ar & boost::serialization::base_object<Unit>(*this);
       ar & mConstant;
   }

private:
   ConstantGenerator(){}
   std::vector<Message> process(Context& ctx, const std::vector<Message>& messages) override;
   void printDetails() const override;
   Data mConstant;
};

// Generates random message with random chance
class RandomGenerator : public Unit
{
public:
   friend class boost::serialization::access;
   RandomGenerator(const UnitId id);
   std::shared_ptr<Unit> clone(const UnitId newId) const override;
   void mutate() override;

   static std::shared_ptr<Unit> createRandom(const UnitId id);

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
       ar & boost::serialization::base_object<Unit>(*this);
       ar & mChance;
       ar & mLen;
   }

private:
   RandomGenerator(){}
   std::vector<Message> process(Context& ctx, const std::vector<Message>& messages) override;
   void printDetails() const override;
   double mChance = 0.0;
   std::size_t mLen = 0;
};

// Generates constant message each X frame
class PhasicGenerator : public Unit
{
public:
   friend class boost::serialization::access;
   PhasicGenerator(const UnitId id);
   std::shared_ptr<Unit> clone(const UnitId newId) const override;
   void mutate() override;

   static std::shared_ptr<Unit> createRandom(const UnitId id);

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
       ar & boost::serialization::base_object<Unit>(*this);
       ar & mStep;
       ar & mPhase;
       ar & mConstant;
   }

private:
   PhasicGenerator(){}
   std::vector<Message> process(Context& ctx, const std::vector<Message>& messages) override;
   void printDetails() const override;
   std::uint8_t mStep = 0;
   std::uint8_t mPhase = 0;
   Data mConstant;
};

// Stores/Restores data
class Storage : public Unit
{
public:
   friend class boost::serialization::access;
   Storage(const UnitId id);
   std::shared_ptr<Unit> clone(const UnitId newId) const override;
   void mutate() override;

   static std::shared_ptr<Unit> createRandom(const UnitId id);

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
       ar & boost::serialization::base_object<Unit>(*this);
       ar & mSlot;
   }

private:
   Storage(){}
   std::vector<Message> process(Context& ctx, const std::vector<Message>& messages) override;
   void printDetails() const override;
   Data mSlot;
};

// Extracts data
class Extractor : public Unit
{
public:
   friend class boost::serialization::access;
   Extractor(const UnitId id, const std::size_t begin, const std::size_t end);
   std::shared_ptr<Unit> clone(const UnitId newId) const override;
   void mutate() override;

   static std::shared_ptr<Unit> createRandom(const UnitId id);

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
       ar & boost::serialization::base_object<Unit>(*this);
       ar & mBegin;
       ar & mEnd;
   }

private:
   Extractor(){}
   std::vector<Message> process(Context& ctx, const std::vector<Message>& messages) override;
   void printDetails() const override;
   std::size_t mBegin = 0;
   std::size_t mEnd = 0;
};

// Combines data
class Combiner : public Unit
{
public:
   friend class boost::serialization::access;
   Combiner(const UnitId id);
   std::shared_ptr<Unit> clone(const UnitId newId) const override;
   void mutate() override;

   static std::shared_ptr<Unit> createRandom(const UnitId id);

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
       ar & boost::serialization::base_object<Unit>(*this);
   }
private:
   Combiner(){}
   std::vector<Message> process(Context& ctx, const std::vector<Message>& messages) override;
   void printDetails() const override;
};

// Applies bitmask on data
class Filter : public Unit
{
public:
   friend class boost::serialization::access;
   Filter(const UnitId id, const Data& bitmask);
   std::shared_ptr<Unit> clone(const UnitId newId) const override;
   void mutate() override;

   static std::shared_ptr<Unit> createRandom(const UnitId id);

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
       ar & boost::serialization::base_object<Unit>(*this);
       ar & mBitmask;
   }

private:
   Filter(){}
   std::vector<Message> process(Context& ctx, const std::vector<Message>& messages) override;
   void printDetails() const override;
   Data mBitmask;
};

// Propagates 1 if input1 matches input2 above threshold, else propagates 0
class Matcher : public Unit
{
public:
   friend class boost::serialization::access;
   Matcher(const UnitId id, const double threshold);
   std::shared_ptr<Unit> clone(const UnitId newId) const override;
   void mutate() override;

   static std::shared_ptr<Unit> createRandom(const UnitId id);

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
       ar & boost::serialization::base_object<Unit>(*this);
       ar & mThreshold;
   }

private:
   Matcher() { }
   std::vector<Message> process(Context& ctx, const std::vector<Message>& messages) override;
   void printDetails() const override;
   double mThreshold = 0.0;
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

   friend class boost::serialization::access;
   LogicalOp(const UnitId id, const Type t);
   std::shared_ptr<Unit> clone(const UnitId newId) const override;
   void mutate() override;

   static std::shared_ptr<Unit> createRandom(const UnitId id);

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
       ar & boost::serialization::base_object<Unit>(*this);
       ar & mType;
   }
private:
   LogicalOp(){}
   std::vector<Message> process(Context& ctx, const std::vector<Message>& messages) override;
   void printDetails() const override;
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

BOOST_CLASS_EXPORT_KEY(sori::CursorManipulator);
BOOST_CLASS_EXPORT_KEY(sori::ScreenReader);
BOOST_CLASS_EXPORT_KEY(sori::ConstantGenerator);
BOOST_CLASS_EXPORT_KEY(sori::RandomGenerator);
BOOST_CLASS_EXPORT_KEY(sori::PhasicGenerator);
BOOST_CLASS_EXPORT_KEY(sori::Storage);
BOOST_CLASS_EXPORT_KEY(sori::Extractor);
BOOST_CLASS_EXPORT_KEY(sori::Combiner);
BOOST_CLASS_EXPORT_KEY(sori::Filter);
BOOST_CLASS_EXPORT_KEY(sori::Matcher);
BOOST_CLASS_EXPORT_KEY(sori::LogicalOp);



