#include "EventHandle.h"

EventHandle::EventHandle()
{
	obj_info tem= { 0,0,0,0,0,0,{0,0} };
	char_info = tem;
	wParam = 0;

	m_Key_UP = FALSE;
	m_Key_DOWN = FALSE;
	m_Key_LEFT = FALSE;
	m_Key_RIGHT = FALSE;
	m_Key_BUBBLE = FALSE;
	m_Key_ITEM = FALSE;
}

EventHandle::~EventHandle()
{

}

void EventHandle::check_obj(Send_datatype data)
{
	for (obj_info i : data.object_info) {
		if (i.type == Char_Idle) {
			char_info = i;
			break;
		}
	}
	wParam = data.wParam;
}

void EventHandle::check_key()
{
	if (wParam != 0) {
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
}

obj_info EventHandle::update_char(int x, int y)
{
	char_info.posX += x;
	char_info.posY += y;
	return char_info;
}

void EventHandle::move_char()
{

}

void EventHandle::set_bubble()
{

}

bool EventHandle::return_key_UP() const
{
	return m_Key_UP;
}

bool EventHandle::return_key_DOWN() const
{
	return m_Key_DOWN;
}

bool EventHandle::return_key_LEFT() const
{
	return m_Key_LEFT;
}

bool EventHandle::return_key_RIGHT() const
{
	return m_Key_RIGHT;
}

bool EventHandle::return_key_BUBBLE() const
{
	return m_Key_BUBBLE;
}

bool EventHandle::return_key_ITEM() const
{
	return m_Key_ITEM;
}
