#pragma once
#include <vector>
#include "variant"
#include "expression.hpp"

namespace seg
{

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
    std::vector<double> anchors;
};

struct RandomWeighted
{
    std::vector<double> weigths;
};
 
struct Choise
{
    std::variant<RandomEven, If, Compare, Switch, RandomWeighted> selector;
    std::vector<GraphNode*> options;
};

struct Operation
{
    MemAddress addr;
    Expression expr;
    GraphNode* next;
};

struct Result
{
    unsigned int commandId;
};

using Action = std::variant<Result, Operation>;

struct GraphNode
{
    unsigned int id;
    std::variant<Choise, Action> payload;
};
 
}