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
        Out operator* () const
        {
            Out result;

            memset(&result, 0, sizeof(Out));
            for(int i = 0; i < BitLen; ++i)
            {
                result |= ((*mData)[mPos + i] << ((BitLen - 1) - i));
            }

            return result;
        }

        Iterator& operator ++ ()
        {
            mPos += BitLen;
            if(mPos + BitLen > mData->size())
            {
                mPos = mData->size();
            }
            return *this;
        }

        bool operator == (const Iterator& other) const
        {
            return mData == other.mData && mPos == other.mPos;
        }

        Iterator(const Data& data, const std::size_t pos)
            : mData(&data)
            , mPos(pos)
        {
        }

    private:
        const Data* mData;
        std::size_t mPos;
    };

    DataReader(const Data& data)
    : mData(data)
    {
    }

    Iterator begin() const
    {
        return Iterator(mData, 0);
    }

    Iterator end() const
    {
        return Iterator(mData, mData.size());
    }

private:
    const Data& mData;
};

template<class In, int BitLen = sizeof(In) * 8>
class DataWritter
{
public:
    DataWritter(Data& data)
        : mData(data)
    {
    }

    void operator()(const In in)
    {
        for(int i = 0; i < BitLen; ++i)
        {
            mData.push_back((in >> ((BitLen - 1) - i)) & 0x1);
        }
        mPos += BitLen;
    }

private:
    Data& mData;
    std::size_t mPos = 0;
};
}
