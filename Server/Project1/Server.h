#pragma once
#include "Haders.h"

DWORD WINAPI ClientThread(LPVOID arg);
DWORD WINAPI ObjectThread(LPVOID arg);

class ServerMain;

extern ServerMain* server;

extern void err_display(const char *b);

extern int WSAGetLastError();

const int Buf_lenth = 81;

class ServerMain
{
private:
	int ClientNum;

	WORD* recv_Buf[2];
	char	send_Buf[Buf_lenth];
public:
	ServerMain();
	~ServerMain();

	void GameServer();

	void DisconnectClient(int iClient);

	void ObjectCollision();

	void KeyCheckClass();

	bool ClientFullCheck();

	bool WaitEventCheck();

	int ClientNum();
};


class ClientMain
{
private:
	char* recv_Buf;
	char send_Buf[Buf_lenth];

	SOCKET* sockets;

	int iClient;

public:
	ClientMain(SOCKET* recv_socket, int i);
	~ClientMain();

	int getClientNum() const { return iClient; }
};