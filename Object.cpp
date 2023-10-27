#include "Object.h"

Object::Object()
{
	posX = 0; posY = 0;
	velX = 0; velY = 0;
	accX = 0; accY = 0;
	type = 0;
}

Object::~Object()
{

}

void Object::SetPosition(int x, int y)
{
	posX = x; posY = y;
}

void Object::SetVelicity(int x, int y)
{
	velX = x; velY = y;
}

void Object::SetAcceleration(int x, int y)
{
	accX = x, accY = y;
}

void Object::SetType(int t)
{
	type = t;
}

void Object::Update()
{

}

void Object::Draw(HINSTANCE hInst, Render* Renderer)
{
	HBITMAP hBit;
	switch (type)
	{
	// Character
	case 1:	
	case 2:
	case 3:
	case 4:
	case 5:
		hBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
		Renderer->Draw(posX, posY, type, hBit);
		break;
	
	// Bubble
	case 6:	
	case 7:
		hBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP6));
		Renderer->Draw(posX, posY, type, hBit);
		break;
	
	// Wall
	case 10:
	case 11:	
	case 12:
	case 13:
	case 14:
		hBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP9));
		Renderer->Draw(posX, posY, type, hBit);
		break;
	case -1:
	default:
		break;
	}
}
