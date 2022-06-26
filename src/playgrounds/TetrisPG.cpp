#include "TetrisPG.hpp"
#include "gacommon/neuro_net2.hpp"
#include "gacommon/rng.hpp"
#include <boost/random.hpp>
#include <future>
//#include <nana/gui.hpp>
//#include <nana/gui/timer.hpp>
#include "tetris/Tetris.hpp"
#include "logger/Logger.hpp"

class TetrisPlayer : public IPlayer
{
public:
   TetrisPlayer(gacommon::IAgent& n)
   : mAgent(n)
   {

   }

   IPlayer::Action getNextAction(bool view[BOARD_WIDTH][BOARD_HEIGHT]) override
   {
       //Reimplement
       throw -1;
      /*switch(mAgent.run(view))
      {
         case 0:
            return IPlayer::Action::MoveLeft;

         case 1:
            return IPlayer::Action::MoveRight;

         case 2:
            return IPlayer::Action::Rotate;

         default:
            return IPlayer::Action::DoNothing;
      }*/

      return IPlayer::Action::DoNothing;
   }

private:
   gacommon::IAgent& mAgent;
};

class FakeIO : public IO
{
public:
   virtual void DrawRectangle(int pX1, int pY1, int pX2, int pY2, enum color pC)
   {

   }

	virtual void ClearScreen()
   {

   }

	virtual int GetScreenHeight()
   {
      return 100;
   }

	virtual int InitGraph()
   {
      return 0;
   }

	virtual void UpdateScreen()
   {

   }
};

nana::color toNanaColor(color c)
{
   switch(c)
   {
      case BLACK:
         return nana::colors::black;

      case RED:
         return nana::colors::red;

      case GREEN:
         return nana::colors::green;

      case BLUE:
         return nana::colors::blue;

      case CYAN:
         return nana::colors::cyan;

      case MAGENTA:
         return nana::colors::magenta;

      case YELLOW:
         return nana::colors::yellow;

      case WHITE:
         return nana::colors::white;

      default:
         return nana::colors::black;
   }
}

class RealIO : public IO
{
public:
   RealIO()
   : mForm(nana::API::make_center(600, 800))
   , mDrawer(mForm)
   , mTimer(std::chrono::milliseconds(50))
   {
      mForm.bgcolor(nana::colors::grey);
      
      mDrawer.draw([&](nana::paint::graphics& graph){

         LOG("Draw called");
         graph.rectangle(true, nana::colors::grey);

         for(auto &r : mDrawObjects)
         {
            graph.rectangle(r.first, true, r.second);
         }
      });

      mTimer.elapse([&](){ LOG("Elapsed"); mDrawer.update(); });
      mTimer.start();
   }

   void start()
   {
      mForm.modality();
   }

   virtual void DrawRectangle(int pX1, int pY1, int pX2, int pY2, enum color pC)
   {
      LOG("New Rect");
      mDrawObjects.push_back({{pX1, pY1, static_cast<unsigned int>(pX2 - pX1), static_cast<unsigned int>(pY2 - pY1)}, 
         toNanaColor(pC)});
   }

	virtual void ClearScreen()
   {
      mDrawObjects.clear();
   }

	virtual int GetScreenHeight()
   {
      LOG("GetScreenHeight");
      return 800;
   }

	virtual int InitGraph()
   {
      return 0;
   }

	virtual void UpdateScreen()
   {
      mDrawer.update();
   }

private:
   std::vector<std::pair<nana::rectangle, nana::color>> mDrawObjects;


   nana::form mForm;
   nana::drawing mDrawer;
   nana::timer mTimer;
};

class TetrisFitnessEvaluator : public gacommon::IFitnessEvaluator
{
public:
   TetrisFitnessEvaluator()
   {
       mSeed = Rng::gen32();
   }

   void step()
   {
       mSeed = Rng::gen32();
   }

   gacommon::Fitness evaluate(gacommon::IAgent& agent) override
   {
      FakeIO io;

      const unsigned int scoreLimit = 10000000;

      gacommon::Fitness result = 0;
      
      agent.reset();

      Tetris t(Mode::AI_Background);
      TetrisPlayer p(agent);

      result = t.run(p, scoreLimit, io, mSeed);

      return result;
   }

private:
   int mGen = 0;
   unsigned int mSeed = 0;
   std::vector<int> mValues;
};

gacommon::IFitnessEvaluator& TetrisPG::getFitnessEvaluator()
{
   return *mFitnessEvaluator;
}

TetrisPG::TetrisPG()
: mFitnessEvaluator(new TetrisFitnessEvaluator)
{
   
}

void TetrisPG::step()
{
   mFitnessEvaluator->step();
}

void TetrisPG::play(gacommon::IAgent& a, std::ostringstream& out)
{  
   RealIO* io = new RealIO();

   auto t = std::thread([&](){
      const unsigned int scoreLimit = 10000000;

      Tetris t(Mode::AI);
      TetrisPlayer p(a);

      t.run(p, scoreLimit, *io, Rng::gen32());
   });

   io->start();
   t.join();
}

std::string TetrisPG::getName() const
{
   return "Tetris";
}

std::vector<gacommon::IOElement> TetrisPG::getInputs() const
{
    throw -1;
}

std::vector<gacommon::IOElement> TetrisPG::getOutputs() const
{
    throw -1;
}
