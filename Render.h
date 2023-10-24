#pragma once

#include "Global.h"

class Render
{
public:
	Render(HDC hdc);
	~Render();

	bool Init();
	void Draw(HDC hdc, HDC memdc, HWND hWnd, HBITMAP hBit, PAINTSTRUCT ps);

private:
	HBRUSH hBrush, oldBrush;
};

