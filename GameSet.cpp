#include "GameSet.h"

GameSet::GameSet(HDC hdc)
{
	m_Renderer = new Render(hdc);
	m_ObjManager = new ObjManager();


}

GameSet::~GameSet()
{
	delete m_Renderer;
	m_Renderer = NULL;

	delete m_ObjManager;
	m_ObjManager = NULL;
}

void GameSet::DrawAll()
{
	if (m_Renderer != NULL) {
		m_ObjManager->Update();
		m_ObjManager->DrawObj();
	}
}

void GameSet::KeyInput(Obj_Interaction* ui)
{
	int x, y = 0;
	

	if (ui->Is_Key_UP()) {

	}
	if (ui->Is_Key_DOWN()) {

	}
	if (ui->Is_Key_LEFT()) {

	}
	if (ui->Is_Key_RIGHT()) {

	}

	if (ui->Is_Key_BUBBLE()) {

	}
	if (ui->Is_Key_ITEM()) {

	}

	//m_ObjManager->SetObjVel()
}
