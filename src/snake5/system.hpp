#pragma once
#include <gacommon/IPlayground.hpp>
#include <gacommon/natural_selection.hpp>
#include <array>

namespace snake5
{

class Snake5System
{
public:
    using ValueType = std::variant<bool, double, std::vector<std::uint8_t>>;
    using ExprId = std::size_t;
    static constexpr ExprId INVALID_ID = 0;

    enum class ExpressionType
    {
        InputTranslate,
        OutputTranslate,
        Constant,
        //Random,
        BinaryOr,
        BinaryAnd,
        BinaryNot,
        LogicalAnd,
        LogicalOr,
        LogicalNot,
        Greater,
        Less,
        Equals,
        Plus,
        Minus,
        Multiply,
        Divide,
        //Match
        Select
    };

    struct Expression
    {
        ExprId id;
        ExpressionType type;

        ExprId in1id = INVALID_ID;
        ExprId in2id = INVALID_ID;
        ExprId in3id = INVALID_ID;
        ValueType constant = false;
    };

    std::unique_ptr<gacommon::IAgent> createAgentImpl(const gacommon::IODefinition& io)
    {
        return std::make_unique<Agent>(mBlocks);
    }

private:
    struct TypeSupport
    {
        const bool Bool = true;
        const bool Number = true;
        const bool Bitmask = false;
    };

    class Agent : public gacommon::IAgent
    {
    public:
       Agent(const std::vector<Expression>& blocks)
           : mBlocks(blocks)
           , mResults(mBlocks.size())
       {
       }

       void reset() override
       {
           //Stateless as for now
       }

       void run(const std::vector<gacommon::IOElement>& inputs, std::vector<gacommon::IOElement>& outputs) override
       {
           for(std::size_t i = 0; i < mBlocks.size(); ++i)
           {
               const auto& block = mBlocks[i];
               auto& result = mResults[i];
               switch(bl.type)
               {
                   case ExpressionType::InputTranslate:
                       result = translateInput(block.in1id);
                       break;

                   case ExpressionType::OutputTranslate:
                       translateOutput(mResults[block.in1id], block.in2id);
                       break;

                   case ExpressionType::BinaryAnd:
                       result = binaryAnd(mResults[block.in1id], mResults[block.in2id]);
                       break;

                   case ExpressionType::BinaryOr:
                       result = binaryOr(mResults[block.in1id], mResults[block.in2id]);
                       break;

                   case ExpressionType::BinaryNot:
                       result = binaryNot(mResults[block.in1id]);
                       break;

                   case ExpressionType::Constant:
                       result = block.constant;
                       break;

                   case ExpressionType::Divide:
                       result = divide(mResults[block.in1id], mResults[block.in2id]);
                       break;

                   case ExpressionType::Equals:
                       result = equals(mResults[block.in1id], mResults[block.in2id]);
                       break;

                   case ExpressionType::Greater:
                       result = greater(mResults[block.in1id], mResults[block.in2id]);
                       break;

                   case ExpressionType::Less:
                       result = less(mResults[block.in1id], mResults[block.in2id]);
                       break;

                   case ExpressionType::LogicalAnd:
                       result = logicalAnd(mResults[block.in1id], mResults[block.in2id]);
                       break;

                   case ExpressionType::LogicalOr:
                       result = logicalOr(mResults[block.in1id], mResults[block.in2id]);
                       break;

                   case ExpressionType::LogicalNot:
                       result = logicalNot(mResults[block.in1id]);
                       break;
               }
           }
       }

       void toBinaryStream(std::ofstream& stream) const override
       {
           throw std::runtime_error("Not implemented");
       }

    private:
       const std::vector<Expression>& mBlocks;
       std::vector<ValueType> mResults;
    };

    std::vector<Expression> mBlocks;
    const TypeSupport mTypeSupport;
};


}
