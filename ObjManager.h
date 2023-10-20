#pragma once

#include "Object.h"
#include "Global.h"

class ObjManager
{
public:
	ObjManager();
	~ObjManager();

	int SetObj(	int posX, int posY,
				int velX, int velY,
				int accX, int accY);
	void SetObjVel(int index, int velX, int velY);
	bool DelObj(int index);

	void DrawObj();
	void Update();

private:
	Object* objects[MAX_OBJ_NUM];
};

