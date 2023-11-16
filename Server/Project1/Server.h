#pragma once
#include "Haders.h"
#include "Global.h"

DWORD WINAPI ClientThread(LPVOID arg);
DWORD WINAPI ObjectThread(LPVOID arg);

class ServerMain;

extern ServerMain* server;
extern HANDLE ClientRecvEvent[2];
extern HANDLE InteractiveEvent;

//extern int WSAGetLastError();

const int Buf_lenth = 512;


class ClientMain
{
public:
	ClientMain(SOCKET recv_socket, int i);
	~ClientMain();

	void EnqueueMsg(const EventQueue& msg);
	void ProcessMessages(); 
	void getBuffer(Send_datatype* buf);
	void returnBuffer(Send_datatype* buf);

	int getClientNum() const { return iClient; }
	SOCKET getClientSocket();

private:
	Send_datatype recv_Buf[Buf_lenth];
	Send_datatype send_Buf[Buf_lenth];

	SOCKET sockets;

	int iClient;

	std::queue<EventQueue> messageQueue;
	HANDLE msgEventQueue;
};

class ServerMain
{
public:
	ServerMain();
	~ServerMain();

	void GameServer();
	void DisconnectClient(int iClient);
	void ObjectCollision();
	void KeyCheckClass();

	bool ClientFullCheck();
	bool WaitEventCheck();

	int getClientNum();

private:
	int ClientNum;

	WORD* recv_Buf[2];
	char send_Buf[Buf_lenth];
};
