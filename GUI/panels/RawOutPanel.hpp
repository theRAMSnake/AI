#pragma once
#include <nana/gui/widgets/textbox.hpp>

class ProjectManager;
class Trainer;
class RawOutPanel
{
public:
   RawOutPanel(nana::window parent, ProjectManager& pm, Trainer& t);

private:
   void refresh();

   nana::textbox mTextBox;
   ProjectManager& mPm;
};