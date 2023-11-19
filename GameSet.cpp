#include "GameSet.h"

GameSet::GameSet(HDC hdc)
{
	m_Renderer = new Render();
	m_ObjManager = new ObjManager();
	g_Time = 0;
	key_Input = 0;
	char_ability init = {0, 0};

	for(int i = 0; i < 15; i++) {
		for (int j = 0; j < 15; j++) {
			Background[i][j] = m_ObjManager->SetObj(0 + (j * 52), 0 + (i * 52), 
				0, 0, 0, 0, -1, init);
		}
	}

	// Forest


	Player1_Index = m_ObjManager->SetObj(0, 0,
		0, 0,
		0, 0,
		Char_Idle, init);
	/*Player2_Index = m_ObjManager->SetObj(50, 50,
		0, 0,
		0, 0,
		Char_Idle);*/

	for(int i = 0; i < 5; i++)
	{
		Player1_bubble[i] = m_ObjManager->SetObj(0, 0,
			0, 0,
			0, 0,
			Non_Bubble, init);
		/*Player2_bubble[i] = m_ObjManager->SetObj(0, 0,
			0, 0,
			0, 0,
			Non_Bubble, init);*/
	}

}

GameSet::~GameSet()
{
	delete m_Renderer;
	m_Renderer = NULL;

	delete m_ObjManager;
	m_ObjManager = NULL;
}

void GameSet::DrawAll(HDC hdc, HINSTANCE hInst)
{
	if (m_Renderer != NULL) {
		m_ObjManager->DrawObj(hdc, hInst, m_Renderer);	
	}
}

void GameSet::KeyInput(Obj_Interaction* g_Interaction, WPARAM wParam)
{
	//m_ObjManager->UpdateAll(g_Interaction, wParam);
	g_Interaction->KeyDown(wParam);
	key_Input = g_Interaction->returnInput();
}

const int GameSet::Key_return()
{
	return key_Input;
}

void GameSet::getTime(DWORD Time)
{
	g_Time = Time;
	m_ObjManager->getBubbleTime(g_Time);
}

void GameSet::getObjINFO(Send_datatype* buf)
{
	obj_info* temp = m_ObjManager->getObj_Info();
	buf->object_info.clear();

	for (int i = 0; i < MAX_OBJ_NUM; i++) {
		if(temp[i].posX)
			buf->object_info.push_back(temp[i]);
	}
}