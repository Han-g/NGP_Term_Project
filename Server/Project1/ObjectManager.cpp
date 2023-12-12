#include "ObjectManager.h"

ObjectManager::ObjectManager()
{
	Time = 0.0;
	wParam = 0;
	clientID = 0;

	object_vector.clear();
	buffer.object_info.clear();
	buffer.GameTime = Time;
	buffer.wParam = wParam;

	eventhandle = new EventHandle();
}

ObjectManager::~ObjectManager()
{
	delete eventhandle;
}

void ObjectManager::GameSet(Send_datatype data)
{
	if (!data.object_info.empty()) {
		buffer = data;
		object_vector = data.object_info;
		wParam = data.wParam;
		eventhandle->check_obj(data);
	}
}

Send_datatype ObjectManager::Update()
{
	buffer.wParam = 0;
	return buffer;
}

void ObjectManager::Key_Check()
{
	eventhandle->check_key();
	//std::cout << buffer.wParam << std::endl;
	if (eventhandle->return_key_UP()) {
		buffer = eventhandle->update_char(0, -1);
	}
	if (eventhandle->return_key_DOWN()) {
		buffer = eventhandle->update_char(0, 1);
	}
	if (eventhandle->return_key_LEFT()) {
		buffer = eventhandle->update_char(-1, 0);
	}
	if (eventhandle->return_key_RIGHT()) {
		buffer = eventhandle->update_char(1, 0);
	}
	if (eventhandle->return_key_BUBBLE()) {

	}
	if (eventhandle->return_key_ITEM()) {

	}
}

bool ObjectManager::Object_collision(Send_datatype& other)
{
	WORD Coll = 5;

	int otherBackX = other.object_info[0].posX - Obj_SizeX / 2;
	int otherBackY = other.object_info[0].posY - Obj_SizeY / 2;
	int otherForX = other.object_info[0].posX + Obj_SizeX / 2;
	int otherForY = other.object_info[0].posY + Obj_SizeY / 2;

	int MyBackX = GetPosition().posX - Obj_SizeX / 2;
	int MyBackY = GetPosition().posY - Obj_SizeY / 2;
	int MyForX = GetPosition().posX + Obj_SizeX / 2;
	int MYForY = GetPosition().posY + Obj_SizeY / 2;

	int disX = GetPosition().posX - other.object_info[0].posX;
	int disY = GetPosition().posY - other.object_info[0].posY;

	if (MyBackX > otherForX) return false;
	if (MyBackY > otherForY) return false;
	if (MyForX < otherBackX) return false;
	if (MYForY < otherBackY) return false;

	if (disX * disX <= disY * disY)
	{
		if (disY < 0)
		{
			Coll |= CollUp;
		}
		else
		{
			Coll |= CollDown;
		}
	}
	else if (disX * disX <= disY * disY)
	{
		if (disX < 0)
		{
			Coll |= CollRight;
		}
		else
		{
			Coll |= CollLeft;
		}
	}

	if (ReturnObjType() == Char_Idle && other.object_info[0].type == Bg_tile1 || ReturnObjType() == Char_Idle && other.object_info[0].type == Bg_tile2
		|| ReturnObjType() == Char_Idle && other.object_info[0].type == Bg_tile3)
	{
		if (Coll == CollLeft)
		{
			buffer.object_info[0].posX = other.object_info[0].posX + Obj_SizeX / 2;
		}
		else if (Coll == CollRight)
		{
			buffer.object_info[0].posX = other.object_info[0].posX - Obj_SizeX / 2;
		}

		else if (Coll == CollUp)
		{
			buffer.object_info[0].posY = other.object_info[0].posY + Obj_SizeY / 2;
		}
		else if (Coll == CollDown)
		{
			buffer.object_info[0].posY = other.object_info[0].posY - Obj_SizeY / 2;
		}
		else
		{
			Coll |= 5;
		}
	}
}

void ObjectManager::getClientID(int id)
{
	clientID = id;
	return;
}
