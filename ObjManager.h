#pragma once

#include "Render.h"
#include "Object.h"
#include "EventHandle.h"
#include "Global.h"

class ObjManager
{
public:
	ObjManager();
	~ObjManager();

	int SetObj(	int posX, int posY,
				int velX, int velY,
				int accX, int accY,
				int type);
	void SetObjVel(int index, int velX, int velY);
	bool DelObj(int index);

	void DrawObj(HINSTANCE hInst, Render* renderer);
	void Update();

private:
	Object* objects[MAX_OBJ_NUM];
};

