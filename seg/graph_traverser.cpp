#include "graph_traverser.hpp"
#include "functions.hpp"
#include <neuroevolution/rng.hpp>

namespace seg
{

GraphTraverser::GraphTraverser(
    const Graph& graph,
    Memory& memory
    )
: mGraph(graph)
, mMemory(memory)
{
    
}

unsigned int GraphTraverser::traverseOnce(const double* input)
{
    auto primitives = createPrimitivesLibrary();
    auto extensions = createExtensionLibrary();
    Interpreter inter(mMemory, input, primitives, extensions, FunctionLibrary());

    NodeId curNodeId = mGraph.root().id;

    for(std::size_t i = 0; i < mGraph.size() * 10; ++i)
    {
        auto& curNode = mGraph.get(curNodeId);

        if(std::holds_alternative<Action>(curNode.payload))
        {
            auto& action = std::get<Action>(curNode.payload);
            
            if(std::holds_alternative<Result>(action))
            {
                return std::get<Result>(action).commandId;
            }
            else if(std::holds_alternative<Operation>(action))
            {
                auto& op = std::get<Operation>(action);

                mMemory.put(op.addr.addr, inter.resolve(op.expr));

                curNodeId = op.next;
            }
            else
            {
                //Unknown action
                throw -1;
            }
        }
        else if(std::holds_alternative<Choise>(curNode.payload))
        {
            auto& ch = std::get<Choise>(curNode.payload);
            curNodeId = procChoise(ch, inter);
        }
        else
        {
            //Unknown payload
            throw -1;
        }
    }

    //Decision was not made in reasonable amount of steps or graph is broken
    return 0;
}

const NodeId GraphTraverser::procChoise(const Choise& ch, Interpreter& inter)
{
    if(ch.options.size() == 1)
    {
        return ch.options[0];
    }

    //Switch, RandomWeighted
    if(std::holds_alternative<RandomEven>(ch.selector))
    {
        return ch.options[Rng::genChoise(ch.options.size())];
    }
    else if(std::holds_alternative<If>(ch.selector))
    {
        auto& if_ch = std::get<If>(ch.selector);

        auto v1 = inter.resolve(if_ch.A);
        auto v2 = inter.resolve(if_ch.B);

        switch(if_ch.sign)
        {
            case IfSign::Equals:
                return  v1 == v2 ? ch.options[0] : ch.options[1];

            case IfSign::Greater:
                return  v1 > v2 ? ch.options[0] : ch.options[1];

            case IfSign::Less:
                return  v1 < v2 ? ch.options[0] : ch.options[1];

            case IfSign::NotEquals:
                return  v1 != v2 ? ch.options[0] : ch.options[1];

            default:
                //Unknown sign
                throw -1;
        }
    }
    else if(std::holds_alternative<Compare>(ch.selector))
    {
        auto& cmpch = std::get<Compare>(ch.selector);

        auto v1 = inter.resolve(cmpch.A);
        auto v2 = inter.resolve(cmpch.B);

        if(v1 > v2)
        {
            return ch.options[0];
        }
        else if(v2 > v1)
        {
            return ch.options[1];
        }
        else
        {
            return ch.options[2];
        }
    }
    else if(std::holds_alternative<Switch>(ch.selector))
    {
        auto& sw = std::get<Switch>(ch.selector);

        auto v = inter.resolve(sw.A);

        std::vector<double> distances(ch.options.size(), 0.0);
        for(std::size_t i = 0; i < distances.size(); ++i)
        {
            distances[i] = std::abs(sw.anchors[i] - v);
        }

        auto pos_d = std::distance(distances.begin(), std::min_element(distances.begin(), distances.end()));
        return *(ch.options.begin() + pos_d);
    }   
    else if(std::holds_alternative<RandomWeighted>(ch.selector))
    {
        auto& rw = std::get<RandomWeighted>(ch.selector);

        std::vector<double> weighted(ch.options.size(), 0.0);
        for(std::size_t i = 0; i < weighted.size(); ++i)
        {
            weighted[i] = rw.weights[i] * Rng::genReal();
        }

        auto pos_d = std::distance(weighted.begin(), std::max_element(weighted.begin(), weighted.end()));
        return *(ch.options.begin() + pos_d);
    }
    else 
    {
        //Unknown choise
        throw -1;
    }

    return 0;
}

}