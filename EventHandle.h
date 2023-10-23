#pragma once

#include "Global.h"
#include "Obj_Interaction.h"

class EventHandle
{
public:
	EventHandle(Obj_Interaction* Obj_Interaction, WPARAM wParam);
	~EventHandle();

	void checkEvent(WPARAM wParam);
	void ResetEvent();

private:
	Obj_Interaction* g_Interaction;
};

