#include "ObjManager.h"

ObjManager::ObjManager()
{
	for (int i = 0; i < MAX_OBJ_NUM; i++) {
		objects[i] = NULL;
	}
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

int ObjManager::SetObj(int posX, int posY, int velX, int velY, int accX, int accY, int type)
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

	for (int i = 0; i < MAX_OBJ_NUM; i++)	{
		if(objects[i]) {
			obj_info temp = objects[i]->returninfo();
			if (temp.type >= Char_Idle && temp.type <= Char_Down) {
				if (g_Interaction->Is_Key_UP()) {
					MoveObject(i, temp.posX, temp.posY + temp.velX);
				}

				if (g_Interaction->Is_Key_DOWN()) {
					MoveObject(i, temp.posX, temp.posY - temp.velX);
				}

				if (g_Interaction->Is_Key_LEFT()) {
					MoveObject(i, temp.posX - temp.velY, temp.posY);
				}

				if (g_Interaction->Is_Key_RIGHT()) {
					MoveObject(i, temp.posX + temp.velY, temp.posY);
				}

				if (g_Interaction->Is_Key_BUBBLE()) {
					// 'BUBBLE' 키가 눌려 있을 때의 동작
				}

				if (g_Interaction->Is_Key_ITEM()) {
					// 'ITEM' 키가 눌려 있을 때의 동작
				}
			}
		}
	}

	g_Interaction->KeyUp();
}