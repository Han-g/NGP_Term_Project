#pragma once

#include "Render.h"
#include "Object.h"
#include "EventHandle.h"
#include "Global.h"
#include "Interact_Interface.h"

class ObjManager : public Interact_Interface
{
public:
	ObjManager();
	~ObjManager();

	int SetObj(	int posX, int posY,
				int velX, int velY,
				int accX, int accY,
				int type );
	void SetObjVel(int index, int velX, int velY);
	bool DelObj(int index);

	void DrawObj(HDC hdc, HINSTANCE hInst, Render* renderer);
	void Update(obj_info changedVel);

	void MoveObject(int index, int posX, int posY) {
		obj_info objVel = objects[index]->returninfo();
		objVel.posX += posX;
		objVel.posY += posY;
		objects[index]->SetPosition(objVel.posX, objVel.posY);
	}

	void SetObjectVelocity(int index, int velX, int velY) {
		obj_info objVel = objects[index]->returninfo();
		objVel.velX += velX;
		objVel.velY += velY;
		objects[index]->SetVelicity(velX, velY);
	}
private:
	Object* objects[MAX_OBJ_NUM];
	//EventHandle* eventhandle;
};