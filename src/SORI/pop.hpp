#pragma once
#include "gacommon/IPlayground.hpp"
#include "components.hpp"
#include "task.hpp"
#include <optional>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>

namespace sori
{

struct Fitness
{
    int score = 0;
    std::size_t energyLeft = 0;

    bool operator > (const Fitness& other)
    {
        if(score == other.score)
        {
            return energyLeft > other.energyLeft;
        }

        return score > other.score;
    }
};

class Pop
{
public:
   friend class boost::serialization::access;

   static Pop createMinimal();
   Pop cloneMutated() const;

   void setFitness(const Fitness);
   Fitness getFitness() const;

   void run(const std::size_t energyLimit, const Image& surface, TaskContext& taskContext);

   template<class Archive>
   void serialize(Archive & ar, const unsigned int version)
   {
        ar & mFitness.score;
        ar & mFitness.energyLeft;
        ar & mNextId;
        ar & mUnits;
   }

   void print() const;

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
   mutable std::uint64_t mNextId = 0;

   std::vector<std::shared_ptr<Unit>> mUnits;
};

void savePop(const std::string& filename, const Pop& pop);
Pop loadPop(const std::string& filename);
Pop popFromBinary(const std::vector<std::uint8_t>& in);
std::vector<uint8_t> popToBinary(const Pop& pop);

}
