#pragma once

#include "Global.h"
#include "Obj_Interaction.h"

class EventHandle
{
public:
	EventHandle(WPARAM wParam);
	~EventHandle();

	void checkEvent(WPARAM wParam);
	void ResetEvent();

private:
	Obj_Interaction* g_Interaction;
};

