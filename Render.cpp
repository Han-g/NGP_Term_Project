#include "Render.h"

Render::Render(HDC hdc)
{
	hBrush = CreateSolidBrush(RGB(0, 0, 255));
	oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
	g_hdc = hdc;
	memdc = NULL;
}

Render::~Render()
{

}

bool Render::Init()
{
	return false;
}

void Render::Draw(int type, HBITMAP hBit)
{
	memdc = CreateCompatibleDC(g_hdc);
	SelectObject(memdc, hBit);

	switch(type)
	{
		// tiles -> 52, 52 | char -> 64, 76 | bubble 56, 54
	case 0:	// character
		BitBlt(g_hdc, 0, 0, 64, 76, memdc, 0, 0, SRCCOPY);
		break;
	case 1:	// bubble
		BitBlt(g_hdc, 0, 0, 56, 54, memdc, 0, 0, SRCCOPY);
		break;
	case 2:	// background
		BitBlt(g_hdc, 0, 0, 52, 52, memdc, 0, 0, SRCCOPY);
		break;
	default:
		break;
	}

	hBrush = CreateSolidBrush(RGB(0, 0, 255));
	oldBrush = (HBRUSH)SelectObject(g_hdc, hBrush);
}
