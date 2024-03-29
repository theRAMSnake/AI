#pragma once

#include "Defines.hpp"
#include "IO.h"

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

   virtual Action getNextAction(bool view[BOARD_WIDTH][BOARD_HEIGHT]) = 0;
};

class Tetris
{
public:
   Tetris(Mode m);

   int run(IPlayer& p, const unsigned int scoreLimit, IO& io, const unsigned int seed);

private:
   Mode mMode;
};
