#include "Server.h"

ServerMain* server;
HANDLE ClientRecvEvent[2];
HANDLE InteractiveEvent;

ClientMain::ClientMain(SOCKET* recv_socket, int i)
{

}

ClientMain::~ClientMain()
{

}

SOCKET ClientMain::getClientSocket()
{
	return sockets;
}


ServerMain::ServerMain()
{
	//ClientNum = 0;
	memset(recv_Buf, 0, sizeof(WORD*) * 2);


}

ServerMain::~ServerMain()
{
}

void ServerMain::GameServer()
{
}

void ServerMain::DisconnectClient(int iClient)
{
}

void ServerMain::ObjectCollision()
{
}

void ServerMain::KeyCheckClass()
{
}

bool ServerMain::ClientFullCheck()
{
	return false;
}

bool ServerMain::WaitEventCheck()
{
	return false;
}

int ServerMain::getClientNum()
{
	return ClientNum;
}
