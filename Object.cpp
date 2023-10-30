#include "Object.h"

Object::Object()
{
	info.posX = 0; info.posY = 0;
	info.velX = 0; info.velY = 0;
	info.type = 0;
}

Object::~Object()
{

}

void Object::SetPosition(int x, int y)
{
	info.posX = x; info.posY = y;
}

void Object::SetVelicity(int x, int y)
{
	info.velX = x; info.velY = y;
}

void Object::SetType(int t)
{
	info.type = t;
}

obj_info Object::returninfo()
{
	return info;
}

void Object::Update()
{
	
}

void Object::Draw(HDC hdc, HINSTANCE hInst, Render* Renderer)
{
	HBITMAP hBit;
	switch (info.type)
	{
	// Character
	case 1:	
	case 2:
	case 3:
	case 4:
	case 5:
		hBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
		Renderer->Draw(hdc, info.posX, info.posY, info.type, hBit);
		break;
	
	// Bubble
	case 6:	
	case 7:
		hBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP6));
		Renderer->Draw(hdc, info.posX, info.posY, info.type, hBit);
		break;
	
	// Wall
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
		hBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP9));
		Renderer->Draw(hdc, info.posX, info.posY, info.type, hBit);
		break;
	case -1:
	default:
		break;
	}
}