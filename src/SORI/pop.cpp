#include "pop.hpp"
#include "gacommon/rng.hpp"
#include <boost/mpl/for_each.hpp>
#include <set>

namespace sori
{

Pop Pop::createMinimal()
{
    Pop pop;

    pop.addUnit<ConstantGenerator>(pop.genId());
    pop.addUnit<CursorManipulator>(pop.genId());
    pop.addUnit<ScreenReader>(pop.genId(), Size{5, 5});
    pop.addRandomUnit();

    for(int i = 0; i < 5; ++i)
    {
        pop.addRandomConnection();
    }

    return pop;
}

void Pop::addRandomUnit()
{
    auto pos = Rng::genChoise(AllUnitTypes::size());
    int i = 0;
    boost::mpl::for_each<AllUnitTypes, boost::mpl::make_identity<boost::mpl::_1>>([&i, pos, this](auto arg) {
            if(i++ == pos)
            {
               mUnits.push_back(decltype(arg)::type::createRandom(genId()));
            }
        });
}

void Pop::addRandomConnection()
{
    auto fromPos = Rng::genChoise(mUnits.size());
    auto toPos = Rng::genChoise(mUnits.size());

    mUnits[fromPos]->connect(mUnits[toPos]->getId());
}

UnitId Pop::genId() const
{
    // Do not forget to update during deserialisation
    static std::uint64_t sId = 0;
    return sId++;
}

Pop Pop::cloneMutated() const
{
    constexpr double DESTROY_UNIT_CHANCE = 0.02;
    constexpr double ADD_UNIT_CHANCE = 0.02;
    constexpr double MUTATE_UNIT_CHANCE = 0.05;
    constexpr double ADD_CONNECTION_CHANCE = 0.05;
    constexpr double REMOVE_CONNECTION_CHANCE = 0.05;

    Pop pop;

    std::set<std::uint32_t> removedUnits;
    for(const auto& unit : mUnits)
    {
        if(Rng::genProbability(DESTROY_UNIT_CHANCE))
        {
            removedUnits.insert(unit->getId());
            continue;
        }

        if(Rng::genProbability(ADD_UNIT_CHANCE))
        {
            pop.addRandomUnit();
        }

        auto newUnit = unit->clone(pop.genId());
        if(Rng::genProbability(MUTATE_UNIT_CHANCE))
        {
            newUnit->mutate();
        }

        pop.mUnits.push_back(newUnit);
    }

    for(auto& unit : pop.mUnits)
    {
        if(Rng::genProbability(ADD_CONNECTION_CHANCE))
        {
            unit->connect(mUnits[Rng::genChoise(mUnits.size())]->getId());
        }
        if(Rng::genProbability(REMOVE_CONNECTION_CHANCE))
        {
            auto outs = unit->getNumConnections();
            if(outs != 0)
            {
                unit->removeConnection(Rng::genChoise(outs));
            }
        }
    }

    for(auto id : removedUnits)
    {
        for(auto& c : mUnits)
        {
            c->removeConnections(id);
        }
    }

    return pop;
}

void Pop::setFitness(const Fitness f)
{
    mFitness = f;
}

Fitness Pop::getFitness() const
{
    return mFitness;
}

std::size_t getMessageCost(const Message& c)
{
    // 1 energy cost per 10 bits
    return c.data->size() / 10;
}

void Pop::run(const std::size_t energyLimit, const Image& surface, std::unique_ptr<TaskContext> taskContext)
{
    Context ctx(surface, *taskContext);

    std::size_t energySpent = 0;
    while(true)
    {
        for(auto& unit : mUnits)
        {
            auto delivery = unit->activate(ctx);
            for(const auto& msg : delivery)
            {
                energySpent += getMessageCost(msg);
                if(energySpent >= energyLimit)
                {
                    // Dead
                    mFitness.errorRating = 0;
                    mFitness.energyLeft = 0;
                    return ;
                }

                getUnitById(msg.destination).postMessage(msg);

                if(ctx.isDone())
                {
                    // Done
                    mFitness.errorRating = ctx.getErrorRating();
                    mFitness.energyLeft = energyLimit - energySpent;
                    return ;
                }
            }
        }
    }
}

Unit& Pop::getUnitById(const UnitId id)
{
    return **std::find_if(mUnits.begin(), mUnits.end(), [id](auto x){return x->getId() == id; });
}

}
