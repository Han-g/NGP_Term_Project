#include "Render.h"

Render::Render(HDC hdc)
{
	hBrush = CreateSolidBrush(RGB(0, 0, 255));
	oldBrush = (HBRUSH)SelectObject(hdc, hBrush);
}

Render::~Render()
{

}

bool Render::Init()
{
	return false;
}

void Render::Draw(HDC hdc, HDC memdc, HWND hWnd, HBITMAP hBit, PAINTSTRUCT ps)
{
	hdc = BeginPaint(hWnd, &ps);
	memdc = CreateCompatibleDC(hdc);
	SelectObject(memdc, hBit);

	hBrush = CreateSolidBrush(RGB(0, 0, 255));
	oldBrush = (HBRUSH)SelectObject(hdc, hBrush);


}
