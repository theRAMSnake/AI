#include "neat/neat.hpp"

#include <iostream>
#include <memory>

std::unique_ptr<neat::Neat> current;

class FitnessEvaluator : public neat::IFitnessEvaluator
{
public:
   neat::Fitness evaluate(const neat::Genom& g) override
   {
      throw -1;
   }
};

int main()
{
   std::cout << "Neat shell" << std::endl;

   FitnessEvaluator ev;
   current.reset(new neat::Neat({3, 2, 100}, ev));
}