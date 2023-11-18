#pragma once
#include "Haders.h"
#include "Global.h"

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
	void getBuffer(Send_datatype* buf);
	void returnBuffer(Send_datatype* buf);
	double getTime();

	void GameStart();
	void EventLoop();

	int getClientNum() const { return iClient; }
	SOCKET getClientSocket();

private:
	Send_datatype recv_Buf[Buf_lenth];
	Send_datatype send_Buf[Buf_lenth];

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

	void GameServer();
	void DisconnectClient(int iClient);
	void KeyCheckClass();
	void ObjectCollision();
	void updateTime(std::chrono::time_point<std::chrono::system_clock> gameTime);

	bool ClientFullCheck();
	bool WaitEventCheck();

	int getClientNum();

private:
	int ClientNum;

	Send_datatype recv_Buf[Buf_lenth];
	Send_datatype send_Buf[Buf_lenth];
};
