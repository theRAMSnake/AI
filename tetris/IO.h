
#ifndef _IO_
#define _IO_

// ------ Enums -----

enum color {BLACK, RED, GREEN, BLUE, CYAN, MAGENTA, YELLOW, WHITE, COLOR_MAX}; // Colors

// --------------------------------------------------------------------------------
//									 IO
// --------------------------------------------------------------------------------

class IO
{
public:
	virtual ~IO() {}

	virtual void DrawRectangle(int pX1, int pY1, int pX2, int pY2, enum color pC) = 0;
	virtual void ClearScreen() = 0;
	virtual int GetScreenHeight() = 0;
	virtual int InitGraph() = 0;
	virtual void UpdateScreen() = 0;
};

#endif // _IO_
