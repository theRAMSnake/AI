#pragma once
#include <vector>
#include "variant"
#include "expression.hpp"

namespace seg
{

using NodeId = std::size_t;
struct GraphNode;
enum class IfSign
{
    Less,
    Greater,
    Equals,
    NotEquals
};

struct RandomEven{};
struct If
{
    Expression A;
    Expression B;
    IfSign sign;
};

struct Compare
{
    Expression A;
    Expression B;
};

struct Switch
{
    Expression A;
    std::vector<double> anchors;
};

struct RandomWeighted
{
    std::vector<double> weights;
};
 
struct Choise
{
    std::variant<RandomEven, If, Compare, Switch, RandomWeighted> selector;
    std::vector<NodeId> options;
};

struct Operation
{
    MemAddress addr;
    Expression expr;
    NodeId next;
};

struct Result
{
    unsigned int commandId;
};

using Action = std::variant<Result, Operation>;
using Payload = std::variant<Choise, Action>;
struct GraphNode
{
    unsigned int id;
    Payload payload;
};
 
}