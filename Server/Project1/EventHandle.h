#pragma once
#include "Global.h"

class EventHandle
{
public:
	EventHandle(Send_datatype data);
	~EventHandle();

private:
	obj_info char_info;
};