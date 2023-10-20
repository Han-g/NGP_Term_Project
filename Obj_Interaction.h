#pragma once
class Obj_Interaction
{
public:
	Obj_Interaction();
	~Obj_Interaction();

	void KeyDown(char key, int index);
	void KeyUp(char key, int index);

	bool Is_Key_UP();
	bool Is_Key_DOWN();
	bool Is_Key_LEFT();
	bool Is_Key_RIGHT();

	bool Is_Key_BUBBLE();
	bool Is_Key_ITEM();

private:
	bool m_Key_UP;
	bool m_Key_DOWN;
	bool m_Key_LEFT;
	bool m_Key_RIGHT;

	bool m_Key_BUBBLE;
	bool m_Key_ITEM;
};

