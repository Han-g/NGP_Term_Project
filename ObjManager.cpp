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

int ObjManager::SetObj(int posX, int posY, int velX, int velY, int accX, int accY)
{
	int index = -1;


	return index;
}

void ObjManager::SetObjVel(int index, int velX, int velY)
{
}

bool ObjManager::DelObj(int index)
{
	return false;
}

void ObjManager::DrawObj()
{
}

void ObjManager::Update()
{
}
