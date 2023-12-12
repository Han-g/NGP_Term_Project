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

void ObjectManager::Object_collision()
{

}

void ObjectManager::getClientID(int id)
{
	clientID = id;
	return;
}
