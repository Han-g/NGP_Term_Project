#include "GameSet.h"

GameSet::GameSet(HDC hdc)
{
	m_Renderer = new Render(hdc);
	m_ObjManager = new ObjManager();

	Player1_Index = m_ObjManager->SetObj(0, 0,
		0, 0,
		0, 0,
		0);
	Player2_Index = m_ObjManager->SetObj(0, 0,
		0, 0,
		0, 0,
		0);

	for(int i = 0; i < 5; i++)
	{
		Player1_bubble[i] = m_ObjManager->SetObj(0, 0,
			0, 0,
			0, 0,
			-1);
		Player2_bubble[i] = m_ObjManager->SetObj(0, 0,
			0, 0,
			0, 0,
			-1);
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
		m_ObjManager->Update();
		m_ObjManager->DrawObj(hInst, m_Renderer);	
	}
}