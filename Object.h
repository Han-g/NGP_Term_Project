#pragma once

#include "resource.h"
#include "Render.h"
#include "Global.h"

class Object
{
public:
	Object();
	~Object();

	void SetPosition(int x, int y);
	void SetVelicity(int x, int y);
	void SetType(int t);
	void SetAbility(char_ability ability);
	void PutBubble();
	void SetBubble(int len);

	obj_info returninfo();

	void Update(obj_info buf);
	void Draw(HDC hdc, HINSTANCE hInst, Render* renderer);

private:
	obj_info info;
};