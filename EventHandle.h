#pragma once

#include "Global.h"
#include "ObjManager.h"
#include "Obj_Interaction.h"
#include "Interact_Interface.h"

class ObjManager;

class EventHandle {
public:
	EventHandle(ObjManager* object, WPARAM wParam);
	~EventHandle();

	bool checkEvent();
	void ResetEvent();
	void HandleKeyEvent();

private:
	ObjManager* objManager;
	Obj_Interaction* g_Interaction;
	WPARAM w;
};