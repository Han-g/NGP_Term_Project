#include "EventHandle.h"

EventHandle::EventHandle(WPARAM wParam)
{
	g_Interaction = new Obj_Interaction;
}

EventHandle::~EventHandle()
{
	delete g_Interaction;
}

void EventHandle::checkEvent(WPARAM wParam)
{
	g_Interaction->KeyDown(wParam);
}

void EventHandle::ResetEvent()
{
	g_Interaction->KeyUp();
}
