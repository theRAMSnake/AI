#include "RawOutPanel.hpp"
#include "../Trainer.hpp"
#include "../ProjectManager.hpp"
#include <numeric>
#include <nana/gui/place.hpp>

void printState(IProject& c, std::stringstream& out)
{
   c.getRawOut(out);
}

RawOutPanel::RawOutPanel(nana::window parent, ProjectManager& pm, Trainer& trainer)
: mTextBox(parent)
, mPm(pm)
{
   nana::place layoutPop(parent);
   layoutPop.div("<a>");

   layoutPop.field("a") << mTextBox;
   layoutPop.collocate();

   mTextBox.editable(false);

   pm.signalProjectChanged.connect(std::bind(&RawOutPanel::refresh, this, std::chrono::milliseconds(0)));
   trainer.signalStep.connect(std::bind(&RawOutPanel::refresh, this, std::placeholders::_1));
}

void RawOutPanel::refresh(std::chrono::milliseconds msecs)
{
   std::stringstream s;
   printState(mPm.getProject(), s);

   mTextBox.caption(s.str());
}
