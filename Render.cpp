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

void Render::Draw(int x, int y, int type, HBITMAP hBit)
{
	memdc = CreateCompatibleDC(g_hdc);
	SelectObject(memdc, hBit);

	switch(type)
	{	
		// tiles -> 52, 52 | char -> 64, 76 | bubble 56, 54
	// character
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		BitBlt(g_hdc, x, y, 64, 76, memdc, 0, 0, SRCCOPY);
		break;
	
	// bubble
	case 6:	
	case 7:
		BitBlt(g_hdc, x, y, 56, 54, memdc, 0, 0, SRCCOPY);
		break;
	
	// background
	case 10:
	case 11:	
	case 12:
	case 13:
	case 14:
		BitBlt(g_hdc, x, y, 52, 52, memdc, 0, 0, SRCCOPY);
		break;
	default:
		break;
	}

	hBrush = CreateSolidBrush(RGB(0, 0, 255));
	oldBrush = (HBRUSH)SelectObject(g_hdc, hBrush);
}
