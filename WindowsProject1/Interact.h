#pragma once

#include "Global.h"

class Interact
{
public:
	Interact();
	~Interact();

	void getObjInfo();
	bool checkKeyInput(WPARAM wParam);
	void resetKeyInput();

private:
	bool m_Key_UP;
	bool m_Key_DOWN;
	bool m_Key_LEFT;
	bool m_Key_RIGHT;

	bool m_Key_BUBBLE;
	bool m_Key_ITEM;
};

