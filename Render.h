#pragma once

#include "Global.h"

class Render
{
public:
	Render();
	~Render();

	bool Init();
	void Draw(HDC hdc, int x, int y, int type, HBITMAP hBit);

private:
	HBRUSH hBrush, oldBrush;
	HDC memdc;
};

