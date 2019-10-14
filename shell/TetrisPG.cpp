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
      std::vector<double> input;
      input.reserve(BOARD_WIDTH * BOARD_HEIGHT + 3);

      for(unsigned int i = 0; i < BOARD_WIDTH; ++i)
      {
         for(unsigned int j = 0; j < BOARD_HEIGHT; ++j)
         {
            input.push_back(view[i][j] ? 1.0 : 0.0);
         }
      }

      input.push_back(piece);
      input.push_back(xpos);
      input.push_back(ypos);

      auto result = mNet.activateLongTerm(input);

      auto pos = std::max_element(result.begin(), result.end());
      switch(std::distance(result.begin(), pos))
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
      const unsigned int scoreLimit = 1000000;

      auto n = neat::NeuroNet(g);

      Tetris t(Mode::AI_Background);
      TetrisPlayer p(n);

      return t.run(p, scoreLimit);
   }

private:
   std::vector<int> mValues;
};

neat::Config TetrisPG::getConfig()
{
   neat::Config c;

   c.numInputs = BOARD_WIDTH * BOARD_HEIGHT + 3; //PieceType, X, Y
   c.numOutputs = 4;  // Left, Right, Rotate, DoNothing
   c.initialPopulation = 100;
   c.optimalPopulation = 100;
   c.compatibilityFactor = 3.0;
   c.inheritDisabledChance = 0.75;
   c.perturbationChance = 0.9;
   c.addNodeMutationChance = 0.05;
   c.addConnectionMutationChance = 0.05;
   c.removeConnectionMutationChance = 0.05;
   c.weightsMutationChance = 0.8;
   c.C1_C2 = 1.0;
   c.C3 = 1.0;
   c.startConnected = true;

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
   const unsigned int scoreLimit = 1000000;

   auto n = neat::NeuroNet(g);

   Tetris t(Mode::AI);
   TetrisPlayer p(n);
   
   t.run(p, scoreLimit);
}