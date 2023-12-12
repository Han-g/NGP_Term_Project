#pragma once

#include "EventHandle.h"

class ObjectManager
{
public:
	ObjectManager();
	~ObjectManager();

	void GameSet(Send_datatype data);
	Send_datatype Update();

	void Key_Check();
	void Object_collision();

	void getClientID(int id);

private:
	obj_info temp = { 0,0,0,0,0,0,{0,0} };
	EventHandle* eventhandle;
	Send_datatype buffer;
	std::vector<obj_info> object_vector;
	double Time;
	int wParam, clientID;
};

