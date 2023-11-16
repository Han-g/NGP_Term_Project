#pragma once
#pragma comment(lib, "ws2_32")

#include <winsock2.h>
#include <Windows.h>

#include <iostream>
using namespace std;

#define nPort 9000

#pragma pack(1)

struct PlayerStatus {

	WORD Id_Num; // 캐릭터 식별 번호
	WORD xPos; // 캐릭터 X 포지션
	WORD yPos; // 캐릭터 Y 포지션
	WORD Type;
	
	bool Hit; // 캐릭터 피격 여부
	bool Dead; // 캐릭터 사망 여부

	int Look; // 캐릭터 보는 방향

	PlayerStatus() :
		Id_Num(0), xPos(0), yPos(0), Type(0), Hit(false), Dead(false), Look(0)
	{
	}
	PlayerStatus(WORD id, WORD x, WORD y, WORD type, bool hit, bool dead, int look) :
		Id_Num(id), xPos(x), yPos(y), Type(type), Hit(hit), Dead(dead), Look(look)
	{
	}
};
#pragma pack()

#pragma pack(1)
struct ObjectStatus {
	WORD xPos;
	WORD yPos;
	WORD Type;
	
	bool Broken;

	ObjectStatus() :
		xPos(0), yPos(0), Type(0), Broken(false)
	{
	}
	ObjectStatus(WORD x, WORD y, WORD type, bool brake)
		:xPos(x), yPos(y), Type(type), Broken(brake)
	{
	}
};
#pragma pack()

struct MapStatus {
	WORD Type;
};


class Vector2
{
public:
	int x, y;

public:
	Vector2() {
		x = y = 0;
	}
	Vector2(int _x, int _y) :x(_x), y(_y) {	}
	Vector2(Vector2&& other) { };
	Vector2(const Vector2& other) {
		x = other.x;
		y = other.y;
	}
	Vector2(const Vector2* other) {
		x = other->x;
		y = other->y;
	}
	~Vector2() { }

public:
	Vector2 GetVector2() { return Vector2(x, y); }

	int GetX() const { return x; }
	int GetY() const { return y; }

	void SetX(const int otherx) { x = otherx; }
	void SetY(const int othery) { y = othery; }

public:
	Vector2 operator=(const Vector2& other) {
		x = other.x; y = other.y;
		return Vector2(x, y);
	}

	Vector2 operator+(const Vector2& other) {
		return Vector2(x + other.x, y + other.y);
	}

	Vector2 operator-(const Vector2& other) {
		return Vector2(x - other.x, y - other.y);
	}

	Vector2 operator+=(const Vector2& other) {
		x += other.x; y += other.y;
		return Vector2(x, y);
	}

	Vector2 operator-=(const Vector2& other) {
		x -= other.x; y -= other.y;
		return Vector2(x, y);;
	}

	Vector2 operator/(const int value) {
		x /= value; y /= value;
		return Vector2(x, y);
	}

	Vector2 operator*(const int value) {
		x *= value; y *= value;
		return Vector2(x, y);
	}
};
