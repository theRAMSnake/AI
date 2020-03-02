#include "TetrisPG.hpp"
#include "neuroevolution/neuro_net.hpp"
#include "neuroevolution/rng.hpp"
#include <boost/random.hpp>
#include <future>
#include <nana/gui.hpp>
#include <nana/gui/timer.hpp>
#include "tetris/Tetris.hpp"
#include "logger/Logger.hpp"

class TetrisPlayer : public IPlayer
{
public:
   TetrisPlayer(neuroevolution::IAgent& n)
   : mAgent(n)
   {

   }

   IPlayer::Action getNextAction(bool view[BOARD_WIDTH][BOARD_HEIGHT], int piece, int xpos, int ypos) override
   {
      double viewAdapted[BOARD_WIDTH * BOARD_HEIGHT];
      auto inputIter = viewAdapted;

      for (unsigned int i = 0; i < BOARD_WIDTH; i++)
      {
         for (unsigned int j = 0; j < BOARD_HEIGHT; j++)
         {
            *inputIter = view[i][j] ? 1.0 : 0.0;
            ++inputIter;
         }
      }

      switch(mAgent.run(viewAdapted))
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

      return IPlayer::Action::DoNothing;
   }

private:
   neuroevolution::IAgent& mAgent;
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
       mSeed = Rng::gen32();
   }

   void step()
   {
       mSeed = Rng::gen32();
   }

   neuroevolution::Fitness evaluate(neuroevolution::IAgent& agent) override
   {
      FakeIO io;

      const unsigned int scoreLimit = 10000000;

      neuroevolution::Fitness result = 0;
      
      for (int i = 0; i < 10; ++i)
      {
         agent.reset();

         Tetris t(Mode::AI_Background);
         TetrisPlayer p(agent);

         result += t.run(p, scoreLimit, io, Rng::gen32());
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

void TetrisPG::play(neuroevolution::IAgent& a)
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

neuroevolution::DomainGeometry TetrisPG::getDomainGeometry() const
{
   neuroevolution::DomainGeometry result;

   result.size = {10, 20, 5};
   result.outputs = {{0, 5}, {9, 5}, {5, 5}, {5, 19}};

   for (unsigned int i = 0; i < BOARD_WIDTH; i++)
   {
      for (unsigned int j = 0; j < BOARD_HEIGHT; j++)
      {
         result.inputs.push_back({i, j});
      }
   }

   return result;
}