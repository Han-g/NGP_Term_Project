#pragma once
#include "Haders.h"
#include "Global.h"
#include "EventHandle.h"

DWORD WINAPI ClientThread(LPVOID arg);
DWORD WINAPI ObjectThread(LPVOID arg);

class ObjectMain;

extern ObjectMain* server;
extern HANDLE ClientRecvEvent[2];
extern HANDLE InteractiveEvent;
extern std::chrono::time_point<std::chrono::system_clock> gameTime;;

//extern int WSAGetLastError();

const int Buf_lenth = 512;

class ServerMain
{
public:
	ServerMain(SOCKET recv_socket, int i);
	~ServerMain();

	void EnqueueMsg(const EventQueue& msg);
	void ProcessMessages(); 
	void getBuffer(Send_datatype buf);
	void returnBuffer(Send_datatype buf);
	double getTime();

	void GameStart();
	void EventLoop();

	int getClientNum() const { return iClient; }
	SOCKET getClientSocket();

private:
	Send_datatype recv_Buf;
	Send_datatype send_Buf;

	SOCKET sockets;

	int iClient;

	std::chrono::time_point<std::chrono::system_clock> gameStartTime;
	std::queue<EventQueue> messageQueue;
	HANDLE msgEventQueue;
};

class ObjectMain
{
public:
	ObjectMain();
	~ObjectMain();

	void GameServer(Send_datatype data);
	void DisconnectClient(int iClient);

	void KeyCheckClass();
	void ObjectCollision();
	void updateTime(std::chrono::time_point<std::chrono::system_clock> gameTime);

	bool ClientFullCheck();
	bool WaitEventCheck();

	int getClientNum();

private:
	int ClientNum;

	std::vector<obj_info> object_vector;
	int wParam;

	EventHandle* events;

	bool m_Key_UP;
	bool m_Key_DOWN;
	bool m_Key_LEFT;
	bool m_Key_RIGHT;
	bool m_Key_BUBBLE;
	bool m_Key_ITEM;

	Send_datatype recv_Buf;
	Send_datatype send_Buf;
};
