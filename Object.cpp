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

void Object::Update()
{

}

void Object::Draw(HINSTANCE hInst, Render* Renderer)
{
	HBITMAP hBit;
	switch (type)
	{
	case 0:
		hBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
		Renderer->Draw(hBit);
		break;
	case 1:
		hBit = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP6));
		break;
	case 2:
		break;
	default:
		break;
	}
}
