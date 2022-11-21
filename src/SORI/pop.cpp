#include "pop.hpp"
#include "gacommon/rng.hpp"
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/mpl/for_each.hpp>
#include <set>
#include <iostream>
#include <algorithm>
#include <fstream>

BOOST_CLASS_EXPORT_IMPLEMENT(sori::CursorManipulator);
BOOST_CLASS_EXPORT_IMPLEMENT(sori::ScreenReader);
BOOST_CLASS_EXPORT_IMPLEMENT(sori::ConstantGenerator);
BOOST_CLASS_EXPORT_IMPLEMENT(sori::RandomGenerator);
BOOST_CLASS_EXPORT_IMPLEMENT(sori::PhasicGenerator);
BOOST_CLASS_EXPORT_IMPLEMENT(sori::Storage);
BOOST_CLASS_EXPORT_IMPLEMENT(sori::Extractor);
BOOST_CLASS_EXPORT_IMPLEMENT(sori::Combiner);
BOOST_CLASS_EXPORT_IMPLEMENT(sori::Filter);
BOOST_CLASS_EXPORT_IMPLEMENT(sori::Matcher);
BOOST_CLASS_EXPORT_IMPLEMENT(sori::LogicalOp);

namespace sori
{

Pop Pop::createMinimal()
{
    Pop pop;

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
    return mNextId++;
}

Pop Pop::cloneMutated() const
{
    constexpr double DESTROY_UNIT_CHANCE = 0.02;
    constexpr double ADD_UNIT_CHANCE = 0.02;
    constexpr double MUTATE_UNIT_CHANCE = 0.05;
    constexpr double ADD_CONNECTION_CHANCE = 0.05;
    constexpr double REMOVE_CONNECTION_CHANCE = 0.05;

    Pop pop;
    pop.mNextId = mNextId;

    std::set<std::uint32_t> removedUnits;
    for(const auto& unit : mUnits)
    {
        if(Rng::genProbability(ADD_UNIT_CHANCE))
        {
            pop.addRandomUnit();
        }

        if(Rng::genProbability(DESTROY_UNIT_CHANCE))
        {
            removedUnits.insert(unit->getId());
            continue;
        }

        auto newUnit = unit->clone(unit->getId());
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
            unit->connect(pop.mUnits[Rng::genChoise(pop.mUnits.size())]->getId());
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
        for(auto& c : pop.mUnits)
        {
            c->removeConnections(id);
        }
    }

    //Keep at least three copms
    for(std::size_t i = std::min(3lu, pop.mUnits.size()); i != 0; --i)
    {
        pop.addRandomUnit();
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
    return c.data.size() / 10;
}

void Pop::run(const std::size_t energyLimit, const Image& surface, TaskContext& taskContext)
{
    if(mUnits.empty())
    {
        mFitness.score = 0;
        mFitness.energyLeft = 0;
        return ;
    }

    Context ctx(surface, taskContext);

    std::size_t energySpent = 0;
    while(true)
    {
        for(auto& unit : mUnits)
        {
            auto delivery = unit->activate(ctx);
            for(const auto& msg : delivery)
            {
                energySpent += getMessageCost(msg);
                getUnitById(msg.destination).postMessage(msg);

                if(ctx.isDone())
                {
                    // Done
                    mFitness.score = ctx.getScore();
                    mFitness.energyLeft = energyLimit - energySpent;
                    return ;
                }
            }
            if(energySpent >= energyLimit)
            {
                // Dead
                mFitness.score = ctx.getScore();
                mFitness.energyLeft = 0;
                return ;
            }

            energySpent++;
        }
    }
}

Unit& Pop::getUnitById(const UnitId id)
{
    auto pos = std::find_if(mUnits.begin(), mUnits.end(), [id](auto x){return x->getId() == id; });
    assert(pos != mUnits.end());
    return **pos;
}

void Pop::print() const
{
    std::cout << "[" << std::endl;
    for(auto& u : mUnits)
    {
        u->print();
    }
    std::cout << "]" << std::endl;
}

void savePop(const std::string& filename, const Pop& pop)
{
    std::ofstream stream(filename, std::ios_base::binary | std::ios_base::trunc);
    boost::archive::binary_oarchive ar(stream);
    ar & pop;
}

Pop loadPop(const std::string& filename)
{
    std::ifstream stream(filename, std::ios_base::binary);
    if(stream)
    {
        boost::archive::binary_iarchive ar(stream);
        Pop result;
        ar & result;
        return result;
    }
    throw std::runtime_error("File not found: " + filename);
}

Pop popFromBinary(const std::vector<std::uint8_t>& in)
{
    std::string str;;
    str.resize(in.size());
    std::copy(in.begin(), in.end(), str.begin());

    boost::iostreams::array_source source{str.data(), str.size()};
    boost::iostreams::stream<boost::iostreams::array_source> is{source};
    boost::archive::binary_iarchive ar(is);
    Pop result;
    ar & result;
    return result;
}

std::vector<std::uint8_t> popToBinary(const Pop& pop)
{
    std::string serial_str;
    {
        boost::iostreams::back_insert_device<std::string> inserter(serial_str);
        boost::iostreams::stream<boost::iostreams::back_insert_device<std::string> > s(inserter);
        boost::archive::binary_oarchive ar(s);
        ar & pop;
    }

    std::vector<std::uint8_t> result;
    result.resize(serial_str.size());
    std::copy(serial_str.begin(), serial_str.end(), result.begin());

    return result;
}

}
