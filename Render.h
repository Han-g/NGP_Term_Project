#pragma once

#include "Global.h"

class Render
{
public:
	Render(HDC hdc);
	~Render();

	bool Init();
	void Draw(HBITMAP hBit);

private:
	HBRUSH hBrush, oldBrush;
	HDC g_hdc, memdc;
};

