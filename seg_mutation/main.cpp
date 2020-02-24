#include <iostream>
#include <seg/mutator.hpp>
#include <seg/graph_traverser.hpp>
#include <neuroevolution/rng.hpp>

void printGraph(const seg::Graph& graph)
{
   std::cout << std::endl;
   graph.print();
}

int main()
{
   Rng::seed(time(0));

   double input[5];
   seg::Memory mem(16);
   seg::Graph graph(5);
   seg::GraphTraverser t(graph, mem);
   seg::Mutator m(seg::MutationConfig(), graph, 16, 5, 5);

   while(true)
   {
      t.traverseOnce(input);
      printGraph(graph);

      std::cin.get();

      m.mutate();
   }

   return 0;
}