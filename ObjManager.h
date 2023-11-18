#pragma once

#include "Global.h"
#include "Render.h"
#include "Object.h"
#include "Obj_Interaction.h"
#include "EventHandle.h"
#include "Interact_Interface.h"

class ObjManager : public Interact_Interface
{
public:
	ObjManager();
	~ObjManager();

	int SetObj(	int posX, int posY,
				int velX, int velY,
				int accX, int accY,
				int type, char_ability ability);
	void SetObjVel(int index, int velX, int velY);
	bool DelObj(int index);

	void DrawObj(HDC hdc, HINSTANCE hInst, Render* renderer);
	void UpdateAll(Obj_Interaction* g_Interaction, WPARAM wParam);
	obj_info* getObj_Info();
	void getBubbleTime(DWORD Time);

	void MoveObject(int index, int velX, int velY) {
		obj_info objVel = objects[index]->returninfo();
		objVel.posX += velX;
		objVel.posY += velY;
		objects[index]->SetPosition(objVel.posX, objVel.posY);
	}

	void SetObjectVelocity(int index, int velX, int velY) {
		obj_info objVel = objects[index]->returninfo();
		objVel.velX += velX;
		objVel.velY += velY;
		objects[index]->SetVelicity(velX, velY);
	}

	void SetBubble(int index, int char_num) {
		//obj_info objVel = objects[index]->returninfo();
		obj_info charVel = objects[char_num]->returninfo();

		objects[index]->SetPosition(charVel.posX, charVel.posY);
		objects[index]->SetBubble(charVel.ablility.bubble_len);
		objects[index]->PutBubble();
	}

	void BubblePop(int index) {
		if ((int)bubble_Time > 100) {
			objects[index]->SetType(Non_Bubble);
			bubble_Time = 0;
			--bubble_count;
		}
	}

private:
	Object* objects[MAX_OBJ_NUM];
	int bubble_count, char_index;
	//EventHandle* eventhandle;
	DWORD bubble_Time;
};