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

void Render::Draw(HBITMAP hBit)
{
	memdc = CreateCompatibleDC(g_hdc);
	SelectObject(memdc, hBit);

	BitBlt(g_hdc, 0, 0, 330, 240, memdc, 0, 0, SRCCOPY);

	hBrush = CreateSolidBrush(RGB(0, 0, 255));
	oldBrush = (HBRUSH)SelectObject(g_hdc, hBrush);


}
