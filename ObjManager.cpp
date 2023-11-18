#include "ObjManager.h"

ObjManager::ObjManager()
{
	for (int i = 0; i < MAX_OBJ_NUM; i++) {
		objects[i] = NULL;
	}
	bubble_count = 0;
	bubble_Time = 0;
	char_index = 0;
}

ObjManager::~ObjManager()
{
	for (int i = 0; i < MAX_OBJ_NUM; i++) {
		if (objects[i] != NULL) {
			delete objects[i];
			objects[i] = NULL;
		}
	}
}

int ObjManager::SetObj(int posX, int posY, int velX, int velY, int accX, int accY, 
	int type, char_ability ability)
{
	int index = -1;

	for (int i = 0; i < MAX_OBJ_NUM; i++) {
		if (objects[i] == NULL) {
			index = i;
			break;
		}
	}

	if (index >= 0) {
		objects[index] = new Object();
		objects[index]->SetPosition(posX, posY);
		objects[index]->SetVelicity(velX, velY);
		objects[index]->SetType(type);
		objects[index]->SetAbility(ability);
		return index;
	}

	return index;
}

void ObjManager::SetObjVel(int index, int velX, int velY)
{
	if (objects[index] != NULL) {
		objects[index]->SetVelicity(velX, velY);
	}
}

bool ObjManager::DelObj(int index)
{
	if (objects[index] != NULL) {
		delete objects[index];
		objects[index] = NULL;
		return true;
	}
	return false;
}

void ObjManager::DrawObj(HDC hdc, HINSTANCE hInst, Render* Renderer)
{
	for (int i = 0; i < MAX_OBJ_NUM; i++) {
		if (objects[i] != NULL) {
			objects[i]->Draw(hdc, hInst, Renderer);
		}
	}
}

void ObjManager::UpdateAll(Obj_Interaction* g_Interaction, WPARAM wParam)
{
	g_Interaction->KeyDown(wParam);
	EventHandle g_handle(this, wParam, g_Interaction);

	for (int i = 0; i < MAX_OBJ_NUM; i++)	{
		if(objects[i]) {
			obj_info temp = objects[i]->returninfo();

			if (temp.type >= Char_Idle && temp.type <= Char_Down) {
				g_handle.HandleKeyEvent(i);
				temp = objects[i]->returninfo();
				char_index = i;

				if (g_Interaction->Is_Key_UP()) {
					MoveObject(i, temp.velX, temp.velY);
				}

				if (g_Interaction->Is_Key_DOWN()) {
					MoveObject(i, temp.velX, temp.velY);
				}

				if (g_Interaction->Is_Key_LEFT()) {
					MoveObject(i, temp.velX, temp.velY);
				}

				if (g_Interaction->Is_Key_RIGHT()) {
					MoveObject(i, temp.velX, temp.velY);
				}
			}

			if (temp.type == Non_Bubble) {
				if (g_Interaction->Is_Key_BUBBLE()) {
					if (i == char_index + bubble_count + 1) {
						bubble_count = (bubble_count + 1) % 5;
						SetBubble(i, char_index);
						break;
					}
				}
			}

			if (temp.type == Bubble_Idle || temp.type == Bubble_bomb) {
				BubblePop(i);
			}

			if (g_Interaction->Is_Key_ITEM()) {
				// 'ITEM' 키가 눌려 있을 때의 동작
			}
			

			objects[i]->SetVelicity(0, 0);
		}
	}

	g_Interaction->KeyUp();
}

obj_info* ObjManager::getObj_Info()
{
	obj_info* obj_infos = new obj_info[MAX_OBJ_NUM];

	for (int i = 0; i < MAX_OBJ_NUM; ++i) {
		if (objects[i]) {
			obj_infos[i] = objects[i]->returninfo();
		}
	}

	return obj_infos;
}

void ObjManager::getBubbleTime(DWORD Time)
{
	bubble_Time = Time;
}
