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

	void DrawAll(HDC hdc, HINSTANCE hInst);
	void KeyInput(Obj_Interaction* g_Interaction, WPARAM wParam);
	const int Key_return();
	void getTime(DWORD Time);
	void getObjINFO(Send_datatype* buf);
	void updateObjINFO(Send_datatype buf);

private:
	Render* m_Renderer;
	ObjManager* m_ObjManager;
	DWORD g_Time;
	int key_Input;

	int Player1_Index;
	//int Player2_Index;

	int Player1_bubble[5];
	//int Player2_bubble[5];

	int Background[15][15];
};

