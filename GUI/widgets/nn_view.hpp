#pragma once
#include <memory>
#include <nana/gui.hpp>
#include "neuroevolution/neuro_net.hpp"

class Nn_view
{
public:
   explicit Nn_view(std::unique_ptr<neuroevolution::NeuroNet>&& ann);
   ~Nn_view();

private:
   nana::form mForm;
   nana::drawing mDraw;
   std::unique_ptr<neuroevolution::NeuroNet> mAnn;
};