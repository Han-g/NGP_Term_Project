#include "ObjectManager.h"

ObjectManager::ObjectManager()
{
	Time = 0.0;
	wParam = 0;

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
	return Send_datatype();
}

void ObjectManager::Key_Check()
{
	eventhandle->check_key();
	if (eventhandle->return_key_UP()) {
		eventhandle->move_char(0, -1);
	}
	if (eventhandle->return_key_DOWN()) {
		eventhandle->move_char(0, 1);
	}
	if (eventhandle->return_key_LEFT()) {
		eventhandle->move_char(-1, 0);
	}
	if (eventhandle->return_key_RIGHT()) {
		eventhandle->move_char(1, 0);
	}
	if (eventhandle->return_key_BUBBLE()) {

	}
	if (eventhandle->return_key_ITEM()) {

	}
}

void ObjectManager::Object_collision()
{

}

int ObjectManager::getClientID()
{
	return 0;
}
