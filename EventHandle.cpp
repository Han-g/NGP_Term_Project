#include "EventHandle.h"

EventHandle::EventHandle(Obj_Interaction* Obj_Interact, WPARAM wParam)
{
	g_Interaction = Obj_Interact;
}

EventHandle::~EventHandle()
{
}

void EventHandle::checkEvent(WPARAM wParam)
{
	g_Interaction->KeyDown(wParam);
}

void EventHandle::ResetEvent()
{
	g_Interaction->KeyUp();
}
