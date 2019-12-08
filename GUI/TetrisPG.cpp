#include "TetrisPG.hpp"
#include "neat/neuro_net.hpp"
#include <boost/random.hpp>
#include "../tetris/Tetris.hpp"

class TetrisPlayer : public IPlayer
{
public:
   TetrisPlayer(neat::NeuroNet& n)
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
   neat::NeuroNet& mNet;
};

class TetrisFitnessEvaluator : public neat::IFitnessEvaluator
{
public:
   TetrisFitnessEvaluator()
   {
      
   }

   void step()
   {
      
   }

   neat::Fitness evaluate(const neat::Genom& g) override
   {
      const unsigned int scoreLimit = 10000000;

      neat::Fitness result = 0;

      for(int i = 0; i < 20; ++i)
      {
         auto n = neat::NeuroNet(g);

         Tetris t(Mode::AI_Background);
         TetrisPlayer p(n);

         result += t.run(p, scoreLimit);
      }

      return result;
   }

private:
   std::vector<int> mValues;
};

neat::Config TetrisPG::getConfig()
{
   neat::Config c;

   c.numInputs = BOARD_WIDTH * BOARD_HEIGHT;
   c.numOutputs = 4;  // Left, Right, Rotate, DoNothing
   c.initialPopulation = 1000;
   c.optimalPopulation = 1000;
   c.compatibilityFactor = 5.0;
   c.inheritDisabledChance = 0.5;
   c.perturbationChance = 0.9;
   c.addNodeMutationChance = 0.1;
   c.addConnectionMutationChance = 0.2;
   c.removeConnectionMutationChance = 0.1;
   c.weightsMutationChance = 0.8;
   c.C1_C2 = 1.0;
   c.C3 = 0.3;
   c.startConnected = true;
   c.numThreads = 4;

   return c;
}

neat::IFitnessEvaluator& TetrisPG::getFitnessEvaluator()
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

void TetrisPG::play(const neat::Genom& g)
{
   const unsigned int scoreLimit = 10000000;

   auto n = neat::NeuroNet(g);

   Tetris t(Mode::AI);
   TetrisPlayer p(n);
   
   t.run(p, scoreLimit);
}

std::string TetrisPG::getName() const
{
   return "Tetris";
}