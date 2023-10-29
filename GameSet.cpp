#include "GameSet.h"

GameSet::GameSet(HDC hdc)
{
	m_Renderer = new Render(hdc);
	m_ObjManager = new ObjManager();

	for(int i = 0; i < 15; i++) {
		for (int j = 0; j < 15; j++) {
			Background[i][j] = m_ObjManager->SetObj(0 + (j * 52), 0 + (i * 52), 
				0, 0, 0, 0, -1);
		}
	}

	Player1_Index = m_ObjManager->SetObj(0, 0,
		0, 0,
		0, 0,
		Char_Idle);
	Player2_Index = m_ObjManager->SetObj(50, 50,
		0, 0,
		0, 0,
		Char_Idle);

	for(int i = 0; i < 5; i++)
	{
		Player1_bubble[i] = m_ObjManager->SetObj(0, 0,
			0, 0,
			0, 0,
			Non_Obj);
		Player2_bubble[i] = m_ObjManager->SetObj(0, 0,
			0, 0,
			0, 0,
			Non_Obj);
	}

}

GameSet::~GameSet()
{
	delete m_Renderer;
	m_Renderer = NULL;

	delete m_ObjManager;
	m_ObjManager = NULL;
}

void GameSet::DrawAll(HINSTANCE hInst)
{
	if (m_Renderer != NULL) {
		//m_ObjManager->Update();
		m_ObjManager->DrawObj(hInst, m_Renderer);	
	}
}

void GameSet::KeyInput(Obj_Interaction* ui)
{

}
