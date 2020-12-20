#pragma once
#include <fstream>

namespace neuroevolution
{

class BinaryOutput
{
public:
    BinaryOutput(const std::string& filename)
    : mOfile(filename, std::ios::binary | std::ios::trunc)
    {

    }

    template<class T>
    BinaryOutput& operator << (const std::vector<T>& vec)
    {
        auto sz = vec.size();
        mOfile.write((char*)&sz, sizeof(std::size_t));
        
        for(auto& i : vec)
        {
            (*this) << i;
        }

        return *this;
    }

    BinaryOutput& operator << (const int v)
    {
        mOfile.write((char*)&v, sizeof(v));

        return *this;
    }

    BinaryOutput& operator << (const std::size_t v)
    {
        mOfile.write((char*)&v, sizeof(v));

        return *this;
    }

    BinaryOutput& operator << (const double v)
    {
        mOfile.write((char*)&v, sizeof(v));

        return *this;
    }

private:
    std::ofstream mOfile;
};

class BinaryInput
{
public:
    BinaryInput(const std::string& filename)
    : mIfile(filename, std::ios::binary)
    {

    }

    template<class T>
    BinaryInput& operator >> (std::vector<T>& vec)
    {
        std::size_t size = 0;
        mIfile.read((char*)&size, sizeof(std::size_t));

        vec.clear();
        vec.reserve(size);

        for(std::size_t i = 0; i < size; ++i)
        {
            auto t = T();
            (*this) >> t;
            vec.push_back(t);
        }

        return *this;
    }

    BinaryInput& operator >> (int& v)
    {
        mIfile.read((char*)&v, sizeof(v));
        return *this;
    }

    BinaryInput& operator >> (std::size_t& v)
    {
        mIfile.read((char*)&v, sizeof(v));
        return *this;
    }

    BinaryInput& operator >> (double& v)
    {
        mIfile.read((char*)&v, sizeof(v));
        return *this;
    }

private:
    std::ifstream mIfile;
};

}