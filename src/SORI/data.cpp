#include "data.hpp"

namespace sori
{

std::shared_ptr<Data> cloneData(const std::shared_ptr<Data>& other)
{
    return std::make_shared<Data>(*other);
}

}
