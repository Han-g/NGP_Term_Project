#pragma once

#include "Global.h"
#include "ObjManager.h"
#include "Obj_Interaction.h"
#include "Interact_Interface.h"

class ObjManager;

class EventHandle {
public:
	EventHandle(ObjManager* object, WPARAM wParam, Obj_Interaction* Interact);
	~EventHandle();

	bool checkEvent();
	void ResetEvent();
	void HandleKeyEvent(int index);

private:
	ObjManager* objManager;
	Obj_Interaction* g_Interaction;
	WPARAM w;
};