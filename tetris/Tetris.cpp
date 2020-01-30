#include "Tetris.hpp"
#include "Pieces.h"
#include "Board.h"
#include "Game.h"
#include "IO.h"
#include <iostream>
#include <thread>
#include <algorithm>

Tetris::Tetris(Mode m)
: mMode(m)
{
   
}

int Tetris::run(IPlayer& p, const unsigned int scoreLimit, IO& io, const unsigned int seed)
{
    srand(seed);
   int screenHeight = io.GetScreenHeight();

   Pieces pieces;
	Board board (&pieces, screenHeight);
	Game game (&board, &pieces, screenHeight, false);
 
   unsigned int score = 0;

   bool view[BOARD_WIDTH][BOARD_HEIGHT];

   while(true)
   {
      if(mMode == Mode::AI)
      {
         std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }

      for(int k = 0; k < 5; ++k)
      {
         for (unsigned int i = 0; i < BOARD_WIDTH; i++)
         for (unsigned int j = 0; j < BOARD_HEIGHT; j++)
            view[i][j] = !board.IsFreeBlock(i, j);

         for (int i = 0; i < PIECE_BLOCKS; i++)
         for (int j = 0; j < PIECE_BLOCKS; j++)
         {
            auto y = game.mPosY + j;
            auto x = game.mPosX + i;
            if(x >= 0 && y >= 0 && x < BOARD_WIDTH && y < BOARD_HEIGHT)
            {
               view[x][y] = view[x][y] || (pieces.GetBlockType (game.mPiece, game.mRotation, i, j) != 0);
            }
         }

         if(mMode == Mode::AI)
         {
            io.ClearScreen();
            game.DrawScene(io);
         }

         auto action = p.getNextAction(view, game.mPiece, game.mPosX + 2, game.mPosY + 2);

         switch(action)
         {
            case IPlayer::Action::MoveLeft:
            {
               if (board.IsPossibleMovement (game.mPosX - 1, game.mPosY, game.mPiece, game.mRotation))
                  game.mPosX--;	
               break;
            }
            case IPlayer::Action::MoveRight:
            {
               if (board.IsPossibleMovement (game.mPosX + 1, game.mPosY, game.mPiece, game.mRotation))
               {
                  game.mPosX++;
               }
               break;
            }
            case IPlayer::Action::Rotate:
            {
               if (board.IsPossibleMovement (game.mPosX, game.mPosY, game.mPiece, (game.mRotation + 1) % 4))
                  game.mRotation = (game.mRotation + 1) % 4;

               break;
            }

            default:
            {
               k = 5;
            }
         }
      }

      if (board.IsPossibleMovement (game.mPosX, game.mPosY + 1, game.mPiece, game.mRotation))
      {
         game.mPosY++;
      }
      else
      {
         board.StorePiece (game.mPosX, game.mPosY, game.mPiece, game.mRotation);
         
         score += board.DeletePossibleLines();

         game.CreateNewPiece();
      }

      if (board.IsGameOver() || score >= scoreLimit)
      {
         return score;
      }

      /*
      Line: +10/30/60/100 pt
      */
   }
}