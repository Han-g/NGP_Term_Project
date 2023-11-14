#pragma once


class Interact_Interface
{
public:
	Interact_Interface();
	~Interact_Interface();

	virtual void MoveObject(int index, int posX, int posY) = 0;
	virtual void SetObjectVelocity(int index, int velX, int velY) = 0;
	virtual void SetBubble(int index, int char_num) = 0;
	virtual void BubblePop(int index) = 0;

private:

};