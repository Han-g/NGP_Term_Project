#include "Obj_Interaction.h"

Obj_Interaction::Obj_Interaction()
{
	m_Key_UP = false;
	m_Key_DOWN = false;
	m_Key_LEFT = false;
	m_Key_RIGHT = false;

	m_Key_BUBBLE = false;
	m_Key_ITEM = false;

	index = 0;
}

Obj_Interaction::~Obj_Interaction()
{

}

void Obj_Interaction::KeyDown(WPARAM wParam)
{
	switch (wParam)
	{
	case 37: // left
		m_Key_LEFT = true;
		break;
	case 38: // up
		m_Key_UP = true;
		break;
	case 39: // right
		m_Key_RIGHT = true;
		break;
	case 40: // down
		m_Key_DOWN = true;
		break;

	case 16: // bubble  (shift)
		m_Key_BUBBLE = true;
		break;
	case 17: // item	 (ctrl)
		m_Key_ITEM = true;
		break;

	default:
		break;
	}
}

void Obj_Interaction::KeyUp()
{
	m_Key_LEFT = false;
	m_Key_UP = false;
	m_Key_RIGHT = false;
	m_Key_DOWN = false;
	m_Key_BUBBLE = false;
	m_Key_ITEM = false;
}

bool Obj_Interaction::Is_Key_UP()
{
	return m_Key_UP;
}

bool Obj_Interaction::Is_Key_DOWN()
{
	return m_Key_DOWN;
}

bool Obj_Interaction::Is_Key_LEFT()
{
	return m_Key_LEFT;
}

bool Obj_Interaction::Is_Key_RIGHT()
{
	return m_Key_RIGHT;
}

bool Obj_Interaction::Is_Key_BUBBLE()
{
	return m_Key_BUBBLE;
}

bool Obj_Interaction::Is_Key_ITEM()
{
	return m_Key_ITEM;
}
