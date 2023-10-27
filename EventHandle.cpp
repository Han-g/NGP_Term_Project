#include "EventHandle.h"

EventHandle::EventHandle(WPARAM wParam)
{
	g_Interaction = new Obj_Interaction;
}

EventHandle::~EventHandle()
{
	delete g_Interaction;
}

bool EventHandle::checkEvent()
{
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

	return false;
}

void EventHandle::ResetEvent()
{
	g_Interaction->KeyUp();
}
