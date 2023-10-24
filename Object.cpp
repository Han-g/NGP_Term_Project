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
	case 0:	// Character
		hBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
		Renderer->Draw(type, hBit);
		break;
	case 1:	// Bubble
		hBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP6));
		Renderer->Draw(type, hBit);
		break;
	case 2:	// Wall
		hBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP6));
		Renderer->Draw(type, hBit);
		break;
	default:
		break;
	}
}
