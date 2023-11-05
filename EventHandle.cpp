#include "EventHandle.h"

EventHandle::EventHandle(ObjManager* objects, WPARAM wParam, Obj_Interaction* Interact)
{
	this->objManager = objects;
	g_Interaction = Interact;
	w = wParam;
}

EventHandle::~EventHandle()
{
	//delete g_Interaction;
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

void EventHandle::HandleKeyEvent(int index)
{
	if (objManager != NULL) {
		if (g_Interaction->Is_Key_UP()) {
			objManager->SetObjectVelocity(index, 0, -1);
		}
		if (g_Interaction->Is_Key_DOWN()) {
			objManager->SetObjectVelocity(index, 0, 1);
		}
		if (g_Interaction->Is_Key_LEFT()) {
			objManager->SetObjectVelocity(index, -1, 0);
		}
		if (g_Interaction->Is_Key_RIGHT()) {
			objManager->SetObjectVelocity(index, 1, 0);
		}

		if (g_Interaction->Is_Key_BUBBLE()) {
			//objManager->SetBubble(index);
		}
		if (g_Interaction->Is_Key_ITEM()) {

		}
	}
}
