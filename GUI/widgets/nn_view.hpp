#pragma once
#include <memory>
#include <nana/gui.hpp>
#include "../../neat/neuro_net.hpp"

class Nn_view
{
public:
   explicit Nn_view(std::unique_ptr<neat::NeuroNet>&& ann);

private:
   nana::form mForm;
   nana::drawing mDraw;
   std::unique_ptr<neat::NeuroNet> mAnn;
};