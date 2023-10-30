#include "EventHandle.h"

EventHandle::EventHandle(ObjManager* objects, WPARAM wParam)
{
	this->objManager = objects;
	g_Interaction = new Obj_Interaction;
	w = wParam;
}

EventHandle::~EventHandle()
{
	delete g_Interaction;
}

bool EventHandle::checkEvent()
{
	if (g_Interaction->KeyDown(w)) {
		if (g_Interaction->Is_Key_UP()) {
			return true;
		}
		if (g_Interaction->Is_Key_DOWN()) {
			return true;
		}
		if (g_Interaction->Is_Key_LEFT()) {
			return true;
		}
		if (g_Interaction->Is_Key_RIGHT()) {
			return true;
		}

		if (g_Interaction->Is_Key_BUBBLE()) {
			return true;
		}
		if (g_Interaction->Is_Key_ITEM()) {
			return true;
		}
	}

	return false;
}

void EventHandle::ResetEvent()
{
	g_Interaction->KeyUp();
}

void EventHandle::HandleKeyEvent()
{
	if (objManager != NULL) {
		if (g_Interaction->Is_Key_UP()) {
			objManager->SetObjectVelocity(225, 0, -1);
		}
		if (g_Interaction->Is_Key_DOWN()) {
			objManager->SetObjectVelocity(225, 0, 1);
		}
		if (g_Interaction->Is_Key_LEFT()) {
			objManager->SetObjectVelocity(255, -1, 0);
		}
		if (g_Interaction->Is_Key_RIGHT()) {
			objManager->SetObjectVelocity(255, 1, 0);
		}

		if (g_Interaction->Is_Key_BUBBLE()) {
			//Interact_interface->MoveObject();
		}
		if (g_Interaction->Is_Key_ITEM()) {
			//Interact_interface->MoveObject();
		}
	}
}
