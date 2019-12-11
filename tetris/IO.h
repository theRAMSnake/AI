
#ifndef _IO_
#define _IO_

// ------ Includes -----

#ifndef WIN32
#include <SDL/SDL.h>									
#include <SDL/SDL_gfxPrimitives.h>
#endif

// ------ Enums -----

enum color {BLACK, RED, GREEN, BLUE, CYAN, MAGENTA, YELLOW, WHITE, COLOR_MAX}; // Colors


// --------------------------------------------------------------------------------
//									 IO
// --------------------------------------------------------------------------------

class IO
{
public:

	IO						();
	~IO();

	void DrawRectangle		(int pX1, int pY1, int pX2, int pY2, enum color pC);
	void ClearScreen		();
	int GetScreenHeight		();
	int InitGraph			();
	int Pollkey				();
	int Getkey				();
	int IsKeyDown			(int pKey);
	void UpdateScreen		();

};

#endif // _IO_
