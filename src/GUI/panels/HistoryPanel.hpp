#pragma once
#include <nana/gui/widgets/panel.hpp>
#include "../widgets/plot.h"

class Trainer;
class ProjectManager;
class HistoryPanel
{
public:
   HistoryPanel(nana::panel<true>& parent, ProjectManager& pm, Trainer& trainer);

private:
   std::unique_ptr<nana::plot::plot> mPlot;
};