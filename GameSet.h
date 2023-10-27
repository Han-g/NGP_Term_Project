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

	void DrawAll(HINSTANCE hInst);
	//void KeyInput(Obj_Interaction* ui);

private:
	Render* m_Renderer;
	ObjManager* m_ObjManager;

	int Player1_Index;
	int Player2_Index;

	int Player1_bubble[5];
	int Player2_bubble[5];

	int Background[15][15];
};

