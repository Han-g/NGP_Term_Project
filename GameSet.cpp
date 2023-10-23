#include "GameSet.h"

GameSet::GameSet(HDC hdc)
{
	m_Renderer = new Render(hdc);
	m_ObjManager = new ObjManager();

	Player1_Index = m_ObjManager->SetObj(0, 0,
		0, 0,
		0, 0,
		1);
	Player2_Index = m_ObjManager->SetObj(0, 0,
		0, 0,
		0, 0,
		1);

	Player1_bubble = m_ObjManager->SetObj(0, 0,
		0, 0,
		0, 0,
		1);
	Player2_bubble = m_ObjManager->SetObj(0, 0,
		0, 0,
		0, 0,
		1);
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

//void GameSet::KeyInput(Obj_Interaction* ui)
//{
//	int x, y = 0;
//	
//
//	if (ui->Is_Key_UP()) {
//
//	}
//	if (ui->Is_Key_DOWN()) {
//
//	}
//	if (ui->Is_Key_LEFT()) {
//
//	}
//	if (ui->Is_Key_RIGHT()) {
//
//	}
//
//	if (ui->Is_Key_BUBBLE()) {
//
//	}
//	if (ui->Is_Key_ITEM()) {
//
//	}
//
//	//m_ObjManager->SetObjVel()
//}
