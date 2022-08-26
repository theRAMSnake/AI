#pragma once
#include "gacommon/IPlayground.hpp"
#include "components.hpp"
#include "task.hpp"
#include <optional>

namespace sori
{

struct Fitness
{
    int errorRating = 0;
    std::size_t energyLeft = 0;

    bool operator > (const Fitness& other)
    {
        if(energyLeft > 0)
        {
            if(other.energyLeft == 0)
            {
                return true;
            }

            if(errorRating == other.errorRating)
            {
                return energyLeft > other.energyLeft;
            }

            return errorRating < other.errorRating;
        }
        else
        {
            return false;
        }
    }
};

class Pop
{
public:
   static Pop createMinimal();
   Pop cloneMutated() const;

   void setFitness(const Fitness);
   Fitness getFitness() const;

   void run(const std::size_t energyLimit, const Image& surface, std::unique_ptr<TaskContext> taskContext);

private:
   template<class Unit, class... params>
   void addUnit(params&& ...args)
   {
       mUnits.push_back(std::make_shared<Unit>(std::forward<params>(args)...));
   }

   void addRandomUnit();
   void addRandomConnection();

   Unit& getUnitById(const UnitId unitId);
   UnitId genId() const;

   Fitness mFitness;

   std::vector<std::shared_ptr<Unit>> mUnits;
};


}
