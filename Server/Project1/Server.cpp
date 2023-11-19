#include "Haders.h"
#include "Server.h"
#include <fstream>

HANDLE ClientRecvEvent[2];
HANDLE InteractiveEvent;
ServerMain* server;




DWORD WINAPI ObjectThread(LPVOID arg)
{
	while (1)
	{
		ResetEvent(InteractiveEvent);

		server->ObjectCollision();
		server->KeyCheckClass();

		SetEvent(InteractiveEvent);
	}return 0;
	
}

DWORD WINAPI ClientThread(LPVOID arg)
{
	ClientMain* thisPoint = (ClientMain*)arg;
	int ClientNum = thisPoint->getClientNum();
	DWORD status;
	while (1)
	{

	}return 0;
}

ServerMain::ServerMain()
{
	ClientNum = 0;
	memset(recv_Buf, 0, sizeof(WORD*) * 2);


}
