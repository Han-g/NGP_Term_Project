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

void ObjManager::DrawObj(HINSTANCE hInst, Render* Renderer)
{
	for (int i = 0; i < MAX_OBJ_NUM; i++) {
		if (objects[i] != NULL) {
			objects[i]->Draw(hInst, Renderer);
		}
	}
}

void ObjManager::Update(obj_info changedVel)
{
	for (int i = 0; i < MAX_OBJ_NUM; i++)	{
		MoveObject(i, changedVel.posX, changedVel.posY);
	}
}