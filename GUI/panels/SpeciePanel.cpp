#include "SpeciePanel.hpp"
#include <filesystem>
#include <nana/gui/place.hpp>
#include <nana/gui/filebox.hpp>
#include <fstream>
#include "../ProjectManager.hpp"
#include "../Trainer.hpp"
#include "timeutils.hpp"

const nana::color COLORS[] = {
   nana::colors::alice_blue ,
   nana::colors::antique_white ,
   nana::colors::aqua	,
   nana::colors::aquamarine ,
   nana::colors::azure	,
   nana::colors::beige	,
   nana::colors::bisque	,
   nana::colors::black	,
   nana::colors::blanched_almond ,
   nana::colors::blue	,
   nana::colors::blue_violet ,
   nana::colors::brown	,
   nana::colors::burly_wood ,
   nana::colors::cadet_blue ,
   nana::colors::chartreuse ,
   nana::colors::chocolate ,
   nana::colors::coral ,
   nana::colors::cornflower_blue ,
   nana::colors::cornsilk ,
   nana::colors::crimson	,
   nana::colors::cyan	,
   nana::colors::dark_blue ,
   nana::colors::dark_cyan ,
   nana::colors::dark_goldenrod ,
   nana::colors::dark_gray ,
   nana::colors::dark_green ,
   nana::colors::dark_grey ,
   nana::colors::dark_khaki ,
   nana::colors::dark_magenta ,
   nana::colors::dark_olive_green ,
   nana::colors::dark_orange ,
   nana::colors::dark_orchid ,
   nana::colors::dark_red ,
   nana::colors::dark_salmon ,
   nana::colors::dark_sea_green ,
   nana::colors::dark_slate_blue ,
   nana::colors::dark_slate_gray ,
   nana::colors::dark_slate_grey ,
   nana::colors::dark_turquoise ,
   nana::colors::dark_violet ,
   nana::colors::deep_pink ,
   nana::colors::deep_sky_blue ,
   nana::colors::dim_gray ,
   nana::colors::dim_grey ,
   nana::colors::dodger_blue ,
   nana::colors::firebrick ,
   nana::colors::floral_white ,
   nana::colors::forest_green ,
   nana::colors::fuchsia	,
   nana::colors::gainsboro ,
   nana::colors::ghost_white ,
   nana::colors::gold ,
   nana::colors::goldenrod ,
   nana::colors::gray ,
   nana::colors::green ,
   nana::colors::green_yellow ,
   nana::colors::grey ,
   nana::colors::honeydew ,
   nana::colors::hot_pink ,
   nana::colors::indian_red ,
   nana::colors::indigo	,
   nana::colors::ivory ,
   nana::colors::khaki ,
   nana::colors::lavendar ,
   nana::colors::lavender_blush ,
   nana::colors::lawn_green ,
   nana::colors::lemon_chiffon ,
   nana::colors::light_blue ,
   nana::colors::light_coral ,
   nana::colors::light_cyan ,
   nana::colors::light_goldenrod_yellow ,
   nana::colors::light_gray ,
   nana::colors::light_green ,
   nana::colors::light_grey ,
   nana::colors::light_pink ,
   nana::colors::light_salmon ,
   nana::colors::light_sea_green ,
   nana::colors::light_sky_blue ,
   nana::colors::light_slate_gray ,
   nana::colors::light_slate_grey ,
   nana::colors::light_steel_blue ,
   nana::colors::light_yellow ,
   nana::colors::lime	,
   nana::colors::lime_green ,
   nana::colors::linen ,
   nana::colors::magenta ,
   nana::colors::maroon	,
   nana::colors::medium_aquamarine ,
   nana::colors::medium_blue ,
   nana::colors::medium_orchid ,
   nana::colors::medium_purple ,
   nana::colors::medium_sea_green ,
   nana::colors::medium_slate_blue ,
   nana::colors::medium_spring_green ,
   nana::colors::medium_turquoise ,
   nana::colors::medium_violet_red ,
   nana::colors::midnight_blue ,
   nana::colors::mint_cream ,

   nana::colors::misty_rose ,
   nana::colors::moccasin ,
   nana::colors::navajo_white ,
   nana::colors::navy	,
   nana::colors::old_lace ,
   nana::colors::olive	,
   nana::colors::olive_drab ,
   nana::colors::orange	,
   nana::colors::orange_red ,
   nana::colors::orchid	,
   nana::colors::pale_goldenrod ,
   nana::colors::pale_green	,
   nana::colors::pale_turquoise ,
   nana::colors::pale_violet_red ,
   nana::colors::papaya_whip ,
   nana::colors::peach_puff ,
   nana::colors::peru	,
   nana::colors::pink	,
   nana::colors::plum	,
   nana::colors::powder_blue ,
   nana::colors::purple	,
   nana::colors::red		,
   nana::colors::rosy_brown ,
   nana::colors::royal_blue ,
   nana::colors::saddle_brown ,
   nana::colors::salmon ,
   nana::colors::sandy_brown ,
   nana::colors::sea_green ,
   nana::colors::sea_shell ,
   nana::colors::sienna	,
   nana::colors::silver	,
   nana::colors::sky_blue ,
   nana::colors::slate_blue ,
   nana::colors::slate_gray ,
   nana::colors::slate_grey ,
   nana::colors::snow	,
   nana::colors::spring_green ,
   nana::colors::steel_blue ,
   nana::colors::tan		,
   nana::colors::teal	,
   nana::colors::thistle	,
   nana::colors::tomato	,
   nana::colors::turquoise ,
   nana::colors::violet	,
   nana::colors::wheat	,
   nana::colors::yellow	,
   nana::colors::yellow_green ,
};

nana::color getColorBySpecieId(const unsigned int id)
{
   return COLORS[id % (sizeof(COLORS) / sizeof(nana::color))];
}

SpeciePanel::SpeciePanel(nana::panel<true>& parent, ProjectManager& pm, Trainer& trainer)
: mInfoTxt(parent)
, mSpecieGroup(parent)
, mPm(pm)
{
   mInfoTxt.editable(false);

   nana::place layoutPop(parent);
   layoutPop.div("<vert a arrange=[20%, 75%]>");

   layoutPop.field("a") << mInfoTxt << mSpecieGroup;
   layoutPop.collocate();

   pm.signalProjectChanged.connect(std::bind(&SpeciePanel::refresh, this, std::chrono::milliseconds(0)));
   trainer.signalStep.connect(std::bind(&SpeciePanel::refresh, this, std::placeholders::_1));

   nana::place layoutSpecie(mSpecieGroup);
   layoutSpecie.div("<a gap=25 margin=10>");

   for(std::size_t i = 0; i < 10; ++i)
   {
      auto g = std::make_shared<nana::group>(mSpecieGroup.handle());
      auto t = std::make_shared<SpecieOverview>(g);

      layoutSpecie.field("a") << *g;

      mSpecieOverviews.push_back(t);
   }
   
   layoutSpecie.collocate();
}

void SpeciePanel::refresh(std::chrono::milliseconds msecs)
{
   auto& pops = mPm.getProject()->getPopulation();

   std::stringstream out;
   out << "Generation: " << mPm.getProject()->getGeneration() << " (" << formatDuration<
      std::chrono::milliseconds,
      std::chrono::minutes,
      std::chrono::seconds,
      std::chrono::milliseconds>(msecs) << ")" << std::endl;
   out << "Total population: " << pops.size() << std::endl;
   out << "Num species: " << pops.getNumSpecies() << std::endl;
   out << "Average fitness: " << pops.getAverageFitness() << std::endl;

   mInfoTxt.caption(out.str());

   std::vector<SpecieResults> bestSpecies;
   for(auto& s : pops.getSpecies())
   {
      if(bestSpecies.size() == 10)
      {
         auto worst = std::min_element(bestSpecies.begin(), bestSpecies.end(), [](auto x, auto y){return x.maxFitness < y.maxFitness;});
         if(worst->maxFitness < s.maxFitness)
         {
            *worst = s;
         }
      }
      else
      {
         bestSpecies.push_back(s);
      }
   }

   std::sort(bestSpecies.begin(), bestSpecies.end(), [](auto& x, auto& y){return x.maxFitness > y.maxFitness;});

   for(std::size_t i = 0; i < 10; ++i)
   {
      auto& overview = *mSpecieOverviews[i];
      if(i >= pops.getNumSpecies())
      {
         overview.hide();
         continue;
      }
      else
      {
         overview.show();
         overview.update(bestSpecies[i], pops.size(), *mPm.getProject());
      }
   }
}

SpecieOverview::SpecieOverview(std::shared_ptr<nana::group> impl)
: mImpl(impl)
, mDrawer(*mImpl)
{
   mImpl->bgcolor(nana::colors::white);
   mDrawer.draw([=](auto graph){
      int margin = 20;
      auto sz = graph.text_extent_size(std::to_string(mTopFitness));
      int availableHeight = graph.height() - margin * 2 - sz.height;
      int barTop = availableHeight - mBarHeightPercentage * availableHeight + margin + sz.height;

      graph.rectangle(nana::rectangle(
            margin, 
            barTop, 
            graph.width() - margin * 2, 
            mBarHeightPercentage * availableHeight), 
            true, 
         getColorBySpecieId(mSpecieId));
      
      graph.string({static_cast<int>(graph.width() / 2 - sz.width / 2), static_cast<int>(barTop - sz.height - 5)}, 
         std::to_string(mTopFitness), 
         nana::colors::blue
         );

      auto bottomText = std::to_string(mSpecieId) + ": " + std::to_string(mNumNodes) + "/" + std::to_string(mComplexity);
      sz = graph.text_extent_size(bottomText);
      graph.string({static_cast<int>(graph.width() / 2 - sz.width / 2), static_cast<int>(graph.height() - sz.height - 5)}, 
         bottomText, 
         nana::colors::black
         );
   });

   mCtx.append ("Export", std::bind(&SpecieOverview::exportAnn, this));
   mCtx.append ("Play", std::bind(&SpecieOverview::play, this));
   mCtx.append ("Visualize", std::bind(&SpecieOverview::visualize, this));
   mImpl->events().mouse_down([&](auto args){
      nana::menu_popuper(mCtx, *mImpl, nana::point(args.pos.x, args.pos.y), nana::mouse::right_button )(args);
   });
}

void SpecieOverview::update(const SpecieResults& specie, const unsigned int totalPop, IProject& prj)
{
   const auto halfOfMaxPops = totalPop / 2;
   mBarHeightPercentage = specie.popResults.size() >= halfOfMaxPops ? 1.0 : static_cast<double>(specie.popResults.size()) / halfOfMaxPops;
   mSpecieId = specie.id;
   mTopFitness = specie.maxFitness;
   mComplexity = specie.popResults[0].complexity;
   mNumNodes = specie.popResults[0].nodeCount;
   mTopPop = specie.popResults[0];
   mPrj = &prj;

   mDrawer.update();
}

void SpecieOverview::show()
{
   mImpl->show();
}

void SpecieOverview::hide()
{
   mImpl->hide();
}

void SpecieOverview::exportAnn()
{  
   nana::filebox fb(*mImpl, false);
   fb.add_filter("ANN file", "*.ann");

   auto items = fb();
   if(!items.empty())
   {
      auto file = items[0];
      
      std::ofstream f;
      f.open(file, std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);

      mPrj->getPopulation().createAnn(mTopPop)->toBinaryStream(f);
      f.close();
   }
}

void SpecieOverview::play()
{
   mPrj->play(*mPrj->getPopulation().createAnn(mTopPop));
}

void SpecieOverview::visualize()
{
   mNnView.reset(new Nn_view(std::move(mPrj->getPopulation().createAnn(mTopPop))));
}