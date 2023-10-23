#pragma once

#include "Render.h"
#include "EventHandle.h"
#include "Object.h"
#include "ObjManager.h"
#include "Obj_Interaction.h"
#include "Global.h"

class GameSet
{
public:
	GameSet(HDC hdc);
	~GameSet();

	void DrawAll();
	void KeyInput(Obj_Interaction* ui);

private:
	Render* m_Renderer;
	ObjManager* m_ObjManager;
};

