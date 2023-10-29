#pragma once

#include "Global.h"
#include "Obj_Interaction.h"
#include "Interact_Interface.h"

class ObjManager;

class EventHandle {
public:
	EventHandle(ObjManager* object, WPARAM wParam);
	~EventHandle();

	bool checkEvent();
	void ResetEvent();
	void HandleKeyEvent(Interact_Interface* Interact_interface);

private:
	ObjManager* objManager;
	Obj_Interaction* g_Interaction;
	WPARAM w;
};