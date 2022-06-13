#include "ProjectBase.hpp"
#include <nana/gui/place.hpp>
#include <nana/gui/screen.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/form.hpp>

const unsigned int ProjectBase::getGeneration() const
{
   return mGeneration;
}

void ProjectBase::setGeneration(const unsigned int generation)
{
   mGeneration = generation;
}

const boost::property_tree::ptree& ProjectBase::getConfig() const
{
   return mConfig;
}

void ProjectBase::updateConfig(const boost::property_tree::ptree& newCfg)
{
   mConfig = newCfg;
   reconfigure();
}

const unsigned int ProjectBase::getAutosavePeriod() const
{
   return mConfig.get<unsigned int>("Basic.Autosave Period");
}

void ProjectBase::play(gacommon::IAgent& agent)
{
   std::ostringstream ss;
   getPlayground().play(agent, ss);

   auto mLastPlayForm = new nana::form{nana::rectangle{nana::size{500, 300}}};
   auto mLastTextBox = new nana::textbox(*mLastPlayForm);
   nana::place layoutPop(*mLastPlayForm);
   layoutPop.div("<a>");

   layoutPop.field("a") << *mLastTextBox;
   layoutPop.collocate();

   mLastTextBox->editable(false);
   mLastTextBox->caption(ss.str());
   mLastPlayForm->show();
}

gacommon::IPlayground& ProjectBase::getPlayground()
{
   return mPlayground;
}

ProjectBase::ProjectBase(const boost::property_tree::ptree& cfg, gacommon::IPlayground& pg)
: mConfig(cfg)
, mPlayground(pg)
{

}
