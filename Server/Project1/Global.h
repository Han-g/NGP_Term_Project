#pragma once

#define MAX_LOADSTRING	100
#define MAX_OBJ_NUM		2000
#define MAX_BUBBLE_NUM	5

#define window_size_w	1040
#define window_size_d	780
#define nPort 9000

#define BUFSIZE 50000

#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include <queue>
#include <chrono>
#include <ctime>
#include <mutex>

// Object status
#define Non_Obj		-1

#define Char_Idle	1
#define Char_Right	2
#define Char_Left	3
#define Char_Up		4
#define Char_Down	5

#define Bubble_Idle	6
#define Bubble_bomb	7
#define Non_Bubble	8

#define BackGround	10
#define Bg_tile1	11
#define Bg_tile2	12
#define Bg_tile3	13
#define Bg_tile4	14

static bool checkStatus(int status) {
	switch (status)
	{
	case Non_Obj:
	case Char_Idle:
	case Char_Right:
	case Char_Left:
	case Char_Up:
	case Char_Down:

	case Bubble_Idle:
	case Bubble_bomb:
	case Non_Bubble:

	case BackGround:
	case Bg_tile1:
	case Bg_tile2:
	case Bg_tile3:
	case Bg_tile4:
		return TRUE;

	default:
		return FALSE;
		break;
	}
}

// Object struct
typedef struct char_ability {
	int bubble_len;
	int bubble_num;
} char_ability;

typedef struct obj_info {
	int posX, posY;
	int velX, velY;
	int type;
	int obj_status;
	char_ability ablility;
} obj_info;

typedef struct Send_datatype {
	std::vector<obj_info> object_info;
	int wParam;
	int GameTime;
} Send_datatype;

struct EventQueue {
	Send_datatype data;
	int msgType;
};

struct ServerData
{
	SOCKET socket;
	Send_datatype buf;
	int sendbytes, recvbytes;
};