#pragma once

#include "Global.h"
#include "Obj_Interaction.h"

class EventHandle
{
public:
	EventHandle(WPARAM wParam);
	~EventHandle();

	bool checkEvent();
	void ResetEvent();

private:
	Obj_Interaction* g_Interaction;
};

