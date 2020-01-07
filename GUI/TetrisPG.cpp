#include "TetrisPG.hpp"
#include "neuroevolution/neuro_net.hpp"
#include "neat/rng.hpp"
#include <boost/random.hpp>
#include <future>
#include <nana/gui.hpp>
#include <nana/gui/timer.hpp>
#include "tetris/Tetris.hpp"
#include "logger/Logger.hpp"

class TetrisPlayer : public IPlayer
{
public:
   TetrisPlayer(neuroevolution::NeuroNet& n)
   : mNet(n)
   {

   }

   IPlayer::Action getNextAction(bool view[BOARD_WIDTH][BOARD_HEIGHT], int piece, int xpos, int ypos) override
   {
      auto inputIter = mNet.begin_input();

      std::copy(&view[0][0], &view[0][0] + BOARD_WIDTH*BOARD_HEIGHT, inputIter);

      mNet.activate();

      auto pos = std::max_element(mNet.begin_output(), mNet.end_output());
      if(*pos > 1.0)
      {
         switch(std::distance(mNet.begin_output(), pos))
         {
            case 0:
               return IPlayer::Action::MoveLeft;

            case 1:
               return IPlayer::Action::MoveRight;

            case 2:
               return IPlayer::Action::Rotate;

            default:
               return IPlayer::Action::DoNothing;
         }
      }

      return IPlayer::Action::DoNothing;
   }

private:
   neuroevolution::NeuroNet& mNet;
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

class TetrisFitnessEvaluator : public neuroevolution::IFitnessEvaluator
{
public:
   TetrisFitnessEvaluator()
   {
       mSeed = neat::Rng::gen32();
   }

   void step()
   {
       mSeed = neat::Rng::gen32();
   }

   neuroevolution::Fitness evaluate(neuroevolution::NeuroNet& ann) override
   {
      FakeIO io;

      const unsigned int scoreLimit = 10000000;

      neuroevolution::Fitness result = 0;
      
      for (int i = 0; i < 10; ++i)
      {
         ann.reset();

         Tetris t(Mode::AI_Background);
         TetrisPlayer p(ann);

         result += t.run(p, scoreLimit, io, neat::Rng::gen32());
      }

      return result;
   }

private:
    int mGen = 0;
    unsigned int mSeed = 0;
   std::vector<int> mValues;
};

neuroevolution::IFitnessEvaluator& TetrisPG::getFitnessEvaluator()
{
   return *mFitnessEvaluator;
}

TetrisPG::TetrisPG()
: mFitnessEvaluator(new TetrisFitnessEvaluator)
{
   
}

unsigned int TetrisPG::getNumInputs() const
{
   return BOARD_WIDTH * BOARD_HEIGHT;
}

unsigned int TetrisPG::getNumOutputs() const
{
   return 4;
}

void TetrisPG::step()
{
   mFitnessEvaluator->step();
}

void TetrisPG::play(neuroevolution::NeuroNet& ann)
{  
   RealIO* io = new RealIO();

   auto t = std::thread([&](){
      const unsigned int scoreLimit = 10000000;

      Tetris t(Mode::AI);
      TetrisPlayer p(ann);

      t.run(p, scoreLimit, *io, neat::Rng::gen32());
   });

   io->start();
   t.join();
}

std::string TetrisPG::getName() const
{
   return "Tetris";
}