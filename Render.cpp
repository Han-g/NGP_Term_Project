#include "Render.h"

Render::Render()
{
	hBrush = CreateSolidBrush(RGB(0, 0, 255));
	oldBrush = CreateSolidBrush(RGB(0, 0, 255));
	memdc = NULL;
}

Render::~Render()
{

}

bool Render::Init()
{
	return false;
}

void Render::Draw(HDC hdc, int x, int y, int type, HBITMAP hBit)
{
	memdc = CreateCompatibleDC(hdc);
	SelectObject(memdc, hBit);
	oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

	switch(type)
	{	
		// tiles -> 52, 52 | char -> 64, 76 | bubble 56, 54
	// character
	case Char_Idle:
	case Char_Right:
	case Char_Left:
	case Char_Up:
	case Char_Down:
		BitBlt(hdc, x * 52, y * 52, 64, 76, memdc, 0, 0, SRCCOPY);
		break;
	
	// bubble
	case Bubble_Idle:
	case Bubble_bomb:
	case Non_Bubble:
		BitBlt(hdc, x * 52, y * 52, 56, 54, memdc, 0, 0, SRCCOPY);
		break;
	
	// background
	case BackGround:
	case Bg_tile1:
	case Bg_tile2:
	case Bg_tile3:
	case Bg_tile4:
		BitBlt(hdc, x * 52, y * 52, 52, 52, memdc, 0, 0, SRCCOPY);
		break;
	default:
		break;
	}

	hBrush = CreateSolidBrush(RGB(0, 0, 255));
	oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
}
