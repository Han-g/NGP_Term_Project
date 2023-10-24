#pragma once

#include "resource.h"
#include "Render.h"
#include "Global.h"

class Object
{
public:
	Object();
	~Object();

	void SetPosition(int x, int y);
	void SetVelicity(int x, int y);
	void SetAcceleration(int x, int y);
	void SetType(int t);

	void Update();
	void Draw(HINSTANCE hInst, Render* renderer);

private:
	int posX, posY = 0;
	int velX, velY = 0;
	int accX, accY = 0;

	int type = 0;
};