#pragma once

#include "Global.h"

class Object
{
public:
	Object();
	~Object();

	void SetPosition(int x, int y);
	void SetVelicity(int x, int y);
	void SetAcceleration(int x, int y);

private:
	int posX, posY = 0;
	int velX, velY = 0;
	int accX, accY = 0;
};

