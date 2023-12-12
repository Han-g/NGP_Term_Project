#pragma once

#include "EventHandle.h"

class ObjectManager
{
public:
	enum CollVec { CollUp = 0, CollDown = 1, CollLeft = 2, CollRight = 3 };
	int Obj_SizeX, Obj_SizeY;

	ObjectManager();
	~ObjectManager();

	void GameSet(Send_datatype data);
	Send_datatype Update();

	void Key_Check();
	obj_info GetPosition() { return this->buffer.object_info[0]; };
	int ReturnObjType() { return this->buffer.object_info[0].type; };
	bool Object_collision(Send_datatype& other);

	void getClientID(int id);

private:
	obj_info temp = { 0,0,0,0,0,0,{0,0} };
	EventHandle* eventhandle;
	Send_datatype buffer;
	std::vector<obj_info> object_vector;
	double Time;
	int wParam, clientID;
};

