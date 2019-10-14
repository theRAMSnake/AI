#pragma once

#include "Defines.hpp"

enum class Mode
{
   AI_Background,
   AI,
   Player
};

class IPlayer
{
public:
   enum class Action
   {
      MoveLeft,
      MoveRight,
      Rotate,
      DoNothing
   };

   virtual Action getNextAction(bool view[BOARD_WIDTH][BOARD_HEIGHT], int piece, int xpos, int ypos) = 0;
};

class Tetris
{
public:
   Tetris(Mode m);

   int run(IPlayer& p, const unsigned int scoreLimit);
};