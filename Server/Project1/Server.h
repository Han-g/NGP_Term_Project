#pragma once
#include "Haders.h"
#include <fstream>

DWORD WINAPI ClientThread(LPVOID arg);
DWORD WINAPI ObjectThread(LPVOID arg);

class ServerMain;

extern ServerMain* server;
extern HANDLE ClientRecvEvent[2];
extern HANDLE InteractiveEvent;

//extern void err_display(const char *b);
//extern int WSAGetLastError();

const int Buf_lenth = 512;


class ClientMain
{
public:
	ClientMain(SOCKET* recv_socket, int i);
	~ClientMain();

	int getClientNum() const { return iClient; }
	SOCKET getClientSocket();

private:
	char* recv_Buf;
	char send_Buf[Buf_lenth];

	SOCKET sockets;

	int iClient;
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
