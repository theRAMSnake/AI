#pragma once
#include <memory>
#include <boost/dynamic_bitset.hpp>

namespace sori
{

using Data = boost::dynamic_bitset<>;

std::shared_ptr<Data> cloneData(const std::shared_ptr<Data>& other);

template<class Out, int BitLen = sizeof(Out) * 8>
class DataReader
{
public:
    class Iterator
    {
    public:
        Out operator* () const;
        Iterator& operator ++ ();
        bool operator == (const Iterator& other) const;
    };

    DataReader(const Data& data);

    Iterator begin() const;
    Iterator end() const;
};

template<class In, int BitLen = sizeof(In) * 8>
class DataWritter
{
public:
    DataWritter(const Data& data);

    void operator()(const In in);

};
}
