#include "Tetris.hpp"
#include "Pieces.h"
#include "Board.h"
#include "Game.h"

#include <iostream>

Tetris::Tetris(Mode m)
{
   
}

int Tetris::run(IPlayer& p, const unsigned int scoreLimit)
{
   Pieces pieces;
	Board board (&pieces, 100);
	Game game (&board, &pieces, 100);

   unsigned int score = 0;

   bool view[BOARD_WIDTH][BOARD_HEIGHT];

   while(true)
   {
      for (unsigned int i = 0; i < BOARD_WIDTH; i++)
		for (unsigned int j = 0; j < BOARD_HEIGHT; j++)
			view[i][j] = board.IsFreeBlock(i, j);

      auto action = p.getNextAction(view, game.mPiece, game.mPosX, game.mPosY);

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

         }
      }

      if (board.IsPossibleMovement (game.mPosX, game.mPosY + 1, game.mPiece, game.mRotation))
      {
         game.mPosY++;
      }
      else
      {
         score++;
         board.StorePiece (game.mPosX, game.mPosY, game.mPiece, game.mRotation);

         auto linesDeleted = board.DeletePossibleLines ();

         switch(linesDeleted)
         {
            case 0: 
            {
               break;
            }
            case 1:
            {
               score += 4;
               break;
            }
            case 2:
            {
               score += 10;
               break;
            }
            case 3:
            {
               score += 30;
               break;
            }
            case 4:
            {
               score += 120;
               break;
            }
            default:
            {
               score += 200;
               break;
            }
         }

         game.CreateNewPiece();
      }

      if (board.IsGameOver() || score >= scoreLimit)
      {
         return score;
      }
   }
}