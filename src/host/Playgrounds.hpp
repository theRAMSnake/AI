#pragma once
#include "gacommon/IPlayground.hpp"

std::unique_ptr<gacommon::IPlayground> createPlayground(const std::string& name);
