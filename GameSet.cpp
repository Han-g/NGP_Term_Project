#include "GameSet.h"

GameSet::GameSet(HDC hdc)
{
	m_Renderer = new Render();
	m_ObjManager = new ObjManager();
	g_Time = 0;
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
	m_ObjManager->UpdateAll(g_Interaction, wParam);
}

void GameSet::getTime(DWORD Time)
{
	g_Time = Time;
	m_ObjManager->getBubbleTime(g_Time);
}
