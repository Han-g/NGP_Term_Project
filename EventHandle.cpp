#include "EventHandle.h"

EventHandle::EventHandle(ObjManager* object, WPARAM wParam)
{
	this->objManager = object;
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

void EventHandle::HandleKeyEvent(Interact_Interface* Interact_interface)
{
	if (Interact_interface != NULL) {
		if (g_Interaction->Is_Key_UP()) {
			
		}
		if (g_Interaction->Is_Key_DOWN()) {
			//Interact_interface->MoveObject();
		}
		if (g_Interaction->Is_Key_LEFT()) {
			//Interact_interface->MoveObject();
		}
		if (g_Interaction->Is_Key_RIGHT()) {
			//Interact_interface->MoveObject();
		}

		if (g_Interaction->Is_Key_BUBBLE()) {
			//Interact_interface->MoveObject();
		}
		if (g_Interaction->Is_Key_ITEM()) {
			//Interact_interface->MoveObject();
		}
	}
}
