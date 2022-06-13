#pragma once
#include <memory>
#include <nana/gui.hpp>
#include "gacommon/neuro_net2.hpp"

class Nn_view
{
public:
   explicit Nn_view(std::unique_ptr<gacommon::IAgent>&& ann);
   ~Nn_view();

private:
   nana::form mForm;
   nana::drawing mDraw;
   std::unique_ptr<gacommon::IAgent> mAnn;
};
