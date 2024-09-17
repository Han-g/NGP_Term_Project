#include "Interact.h"

Interact::Interact()
{
	m_Key_UP = false;
	m_Key_DOWN = false;
	m_Key_LEFT = false;
	m_Key_RIGHT = false;

	m_Key_BUBBLE = false;
	m_Key_ITEM = false;
}

Interact::~Interact()
{

}

void Interact::getObjInfo()
{

}

bool Interact::checkKeyInput(WPARAM wParam)
{
	switch (wParam)
	{
	case 37: // left
		m_Key_LEFT = true;
		return m_Key_LEFT;
	case 38: // up
		m_Key_UP = true;
		return m_Key_UP;
	case 39: // right
		m_Key_RIGHT = true;
		return m_Key_RIGHT;
	case 40: // down
		m_Key_DOWN = true;
		return m_Key_DOWN;

	case 16: // bubble  (shift)
		m_Key_BUBBLE = true;
		return m_Key_BUBBLE;
	case 17: // item	 (ctrl)
		m_Key_ITEM = true;
		return m_Key_ITEM;

	default:
		return false;
	}
	return false;
}

void Interact::resetKeyInput()
{
	m_Key_UP = false;
	m_Key_DOWN = false;
	m_Key_LEFT = false;
	m_Key_RIGHT = false;

	m_Key_BUBBLE = false;
	m_Key_ITEM = false;
}
