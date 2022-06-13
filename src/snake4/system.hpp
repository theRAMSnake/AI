#pragma once
#include <gacommon/IPlayground.hpp>
#include <gacommon/natural_selection.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/split_member.hpp>

namespace snake4
{

struct AlwaysActivator{};

struct SensoricActivatorForValue
{
    enum class Type
    {
        Above,
        Below,
        InRange,
        NotInRange
    };
    static constexpr auto TypeSize = 4;

    std::size_t inputIdx;
    Type type;
    double a;
    double b;
};

struct SensoricActivatorForChoice
{
    std::size_t inputIdx;
    std::vector<std::uint8_t> values;
    bool inversion;
};

using ActivatorDefinition = std::variant<AlwaysActivator, SensoricActivatorForValue, SensoricActivatorForChoice>;

struct SetValueManipulator
{
    enum class FunctionType
    {
        Zero,
        One,
        Inc,
        Dec
    };
    static constexpr auto FunctionTypeSize = 4;

    std::size_t outputIdx;
    FunctionType funcType;
};

struct ChoiceVoteManipulator
{
    std::size_t outputIdx;
    std::size_t selection;
};

using ForceDefinition = std::variant<SetValueManipulator, ChoiceVoteManipulator>;

struct BlockDefinition
{
    ActivatorDefinition activator;
    ForceDefinition force;

    BOOST_SERIALIZATION_SPLIT_MEMBER()

    template <class Archive>
    void save(Archive& ar, const unsigned int version) const
    {
    }

    template <class Archive>
    void load(Archive& ar, const unsigned int version)
    {
    }
};

std::unique_ptr<gacommon::IAgent> createAgentImpl(const gacommon::IODefinition& io, const std::vector<BlockDefinition>& blocks);

}
