
#ifndef _GAME_
#define _GAME_

// ------ Includes -----

#include "Board.h"
#include "Pieces.h"
#include "IO.h"
#include <time.h>

// ------ Defines -----

#define WAIT_TIME 100			// Number of milliseconds that the piece remains before going 1 block down */ 


// --------------------------------------------------------------------------------
//									 Game
// --------------------------------------------------------------------------------

class Game
{
public:

	Game			(Board *pBoard, Pieces *pPieces, int pScreenHeight, bool trainingMode);

	void DrawScene (IO& io);
	void DrawView(IO& io, bool view[BOARD_WIDTH][BOARD_HEIGHT]);
	void CreateNewPiece ();

	int mPosX, mPosY;				// Position of the piece that is falling down
	int mPiece, mRotation;			// Kind and rotation the piece that is falling down

private:

	int mScreenHeight;				// Screen height in pixels
	int mNextPosX, mNextPosY;		// Position of the next piece
	int mNextPiece, mNextRotation;	// Kind and rotation of the next piece
	bool mTrainingMode = false;
	int mNumPiecesCreated = 0;

	Board *mBoard;
	Pieces *mPieces;

	int GetRand (int pA, int pB);
	void InitGame();
	void DrawPiece (int pX, int pY, int pPiece, int pRotation, IO& io);
	void DrawBoard (IO& io);
};

#endif // _GAME_
