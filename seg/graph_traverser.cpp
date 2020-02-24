#include "graph_traverser.hpp"
#include "functions.hpp"

namespace seg
{

GraphTraverser::GraphTraverser(
    const GraphNode& root,
    Memory& memory
    )
: mRoot(root)
, mMemory(memory)
{
    
}

unsigned int GraphTraverser::traverseOnce(const double* input)
{
    auto primitives = createPrimitivesLibrary();
    auto extensions = createExtensionLibrary();
    Interpreter inter(mMemory, input, primitives, extensions, FunctionLibrary());

    const GraphNode* curNode = &mRoot;

    //How do we prevent forever execution of cyclic graph?
    while(true)
    {
        if(std::holds_alternative<Action>(curNode->payload))
        {
            auto& action = std::get<Action>(curNode->payload);
            
            if(std::holds_alternative<Result>(action))
            {
                return std::get<Result>(action).commandId;
            }
            else if(std::holds_alternative<Operation>(action))
            {
                auto& op = std::get<Operation>(action);

                mMemory.put(op.addr.addr, inter.resolve(op.expr));

                curNode = op.next;
            }
            else
            {
                //Unknown action
                throw -1;
            }
        }
        else if(std::holds_alternative<Choise>(curNode->payload))
        {
            auto& ch = std::get<Choise>(curNode->payload);
        }
        else
        {
            //Unknown payload
            throw -1;
        }
    }
}

}