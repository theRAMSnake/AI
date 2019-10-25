#include "Game.h"


/* 
======================================									
Init
====================================== 
*/
Game::Game(Board *pBoard, Pieces *pPieces, int pScreenHeight, bool trainingMode) 
{
	mTrainingMode = trainingMode;
	mScreenHeight = pScreenHeight;

	// Get the pointer to the Board and Pieces classes
	mBoard = pBoard;
	mPieces = pPieces;

	// Game initialization
	InitGame ();
}


/* 
======================================									
Get a random int between to integers

Parameters:
>> pA: First number
>> pB: Second number
====================================== 
*/
int Game::GetRand (int pA, int pB)
{
	return rand () % (pB - pA + 1) + pA;
}


/* 
======================================									
Initial parameters of the game
====================================== 
*/
void Game::InitGame()
{
	// Init random numbers
	srand ((unsigned int) time(NULL));

	// First piece
	mPiece			= mTrainingMode ? 0 : GetRand (0, 6);
	mRotation		= GetRand (0, 3);
	mPosX 			= (BOARD_WIDTH / 2) + mPieces->GetXInitialPosition (mPiece, mRotation);
	mPosY 			= mPieces->GetYInitialPosition (mPiece, mRotation);

	//  Next piece
	mNextPiece 		= mTrainingMode ? 0 : GetRand (0, 6);
	mNextRotation 	= GetRand (0, 3);
	mNextPosX 		= BOARD_WIDTH + 5;
	mNextPosY 		= 5;	

	mNumPiecesCreated++;
}


/* 
======================================									
Create a random piece
====================================== 
*/
void Game::CreateNewPiece()
{
	// The new piece
	mPiece			= mNextPiece;
	mRotation		= mNextRotation;
	mPosX 			= (BOARD_WIDTH / 2) + mPieces->GetXInitialPosition (mPiece, mRotation);
	mPosY 			= mPieces->GetYInitialPosition (mPiece, mRotation);

	// Random next piece
	mNextPiece 		= mTrainingMode ? GetRand (0, std::min(6, mNumPiecesCreated++ / 50)) : GetRand (0, 6);
	mNextRotation 	= GetRand (0, 3);
}

/* 
======================================									
Draw piece

Parameters:

>> pX:		Horizontal position in blocks
>> pY:		Vertical position in blocks
>> pPiece:	Piece to draw
>> pRotation:	1 of the 4 possible rotations
====================================== 
*/
void Game::DrawPiece (int pX, int pY, int pPiece, int pRotation, IO& io)
{
	color mColor = BLACK;				// Color of the block 

	// Obtain the position in pixel in the screen of the block we want to draw
	int mPixelsX = mBoard->GetXPosInPixels (pX);
	int mPixelsY = mBoard->GetYPosInPixels (pY);

	// Travel the matrix of blocks of the piece and draw the blocks that are filled
	for (int i = 0; i < PIECE_BLOCKS; i++)
	{
		for (int j = 0; j < PIECE_BLOCKS; j++)
		{
			// Get the type of the block and draw it with the correct color
			switch (mPieces->GetBlockType (pPiece, pRotation, i, j))
			{
				case 1: mColor = GREEN; break;	// For each block of the piece except the pivot
				case 2: mColor = BLUE; break;	// For the pivot
			}
			
			if (mPieces->GetBlockType (pPiece, pRotation, i, j) != 0)
				io.DrawRectangle	(mPixelsX + i * BLOCK_SIZE, 
									mPixelsY + j * BLOCK_SIZE, 
									(mPixelsX + i * BLOCK_SIZE) + BLOCK_SIZE - 1, 
									(mPixelsY + j * BLOCK_SIZE) + BLOCK_SIZE - 1, 
									mColor);
		}
	}
}


/* 
======================================									
Draw board

Draw the two lines that delimit the board
====================================== 
*/
void Game::DrawBoard (IO& io)
{
	// Calculate the limits of the board in pixels	
	int mX1 = BOARD_POSITION - (BLOCK_SIZE * (BOARD_WIDTH / 2)) - 1;
	int mX2 = BOARD_POSITION + (BLOCK_SIZE * (BOARD_WIDTH / 2));
	int mY = mScreenHeight - (BLOCK_SIZE * BOARD_HEIGHT);
	
	// Check that the vertical margin is not to small
	//assert (mY > MIN_VERTICAL_MARGIN);

	// Rectangles that delimits the board
	io.DrawRectangle (mX1 - BOARD_LINE_WIDTH, mY, mX1, mScreenHeight - 1, BLUE);
	io.DrawRectangle (mX2, mY, mX2 + BOARD_LINE_WIDTH, mScreenHeight - 1, BLUE);
	
	// Check that the horizontal margin is not to small
	//assert (mX1 > MIN_HORIZONTAL_MARGIN);

	// Drawing the blocks that are already stored in the board
	mX1 += 1;
	for (int i = 0; i < BOARD_WIDTH; i++)
	{
		for (int j = 0; j < BOARD_HEIGHT; j++)
		{	
			// Check if the block is filled, if so, draw it
			if (!mBoard->IsFreeBlock(i, j))	
				io.DrawRectangle (	mX1 + i * BLOCK_SIZE, 
										mY + j * BLOCK_SIZE, 
										(mX1 + i * BLOCK_SIZE) + BLOCK_SIZE - 1, 
										(mY + j * BLOCK_SIZE) + BLOCK_SIZE - 1, 
										RED);
		}
	}	
}

void Game::DrawView(IO& io, bool view[BOARD_WIDTH][BOARD_HEIGHT])
{
	constexpr int smallBlockSize = 10;
	int mY = mScreenHeight / 2;

	for (int i = 0; i < BOARD_WIDTH; i++)
	{
		for (int j = 0; j < BOARD_HEIGHT; j++)
		{	
			io.DrawRectangle (	5 + i * smallBlockSize, 
										mY + 5 + j * smallBlockSize, 
										(5 + i * smallBlockSize) + smallBlockSize - 1, 
										mY + (5 + j * smallBlockSize) + smallBlockSize - 1, 
										view[i][j] ? RED : WHITE);
		}
	}
}


/* 
======================================									
Draw scene

Draw all the objects of the scene
====================================== 
*/
void Game::DrawScene (IO& io)
{
	DrawBoard (io);													// Draw the delimitation lines and blocks stored in the board
	DrawPiece (mPosX, mPosY, mPiece, mRotation, io);					// Draw the playing piece
	DrawPiece (mNextPosX, mNextPosY, mNextPiece, mNextRotation, io);	// Draw the next piece
}
