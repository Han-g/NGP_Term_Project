#include "Server.h"

ServerMain* server;
HANDLE ClientRecvEvent[2];
HANDLE InteractiveEvent;

// ServerThread
ClientMain::ClientMain(SOCKET recv_socket, int i)
{
	memset(recv_Buf, 0, sizeof(recv_Buf));
	memset(send_Buf, 0, sizeof(send_Buf));
	sockets = recv_socket;
	iClient = i;

	msgEventQueue = CreateEvent(NULL, FALSE, FALSE, NULL);
}

ClientMain::~ClientMain()
{
	CloseHandle(msgEventQueue);
}

void ClientMain::EnqueueMsg(const EventQueue& msg)
{
	// �޽��� ť�� �޽��� �߰�
	SetEvent(msgEventQueue);
	messageQueue.push(msg);
}

void ClientMain::ProcessMessages()
{
	WaitForSingleObject(msgEventQueue, INFINITE);

	// �޽��� ť�� ������� ������ ó��
	while (!messageQueue.empty()) {
		EventQueue message = messageQueue.front();
		// �޽��� ó�� ���� �߰�

		// �޽��� ť���� �޽��� ����
		messageQueue.pop();
	}
}

void ClientMain::getBuffer(Send_datatype* buf)
{
	memcpy(recv_Buf, &buf, sizeof(Send_datatype));
	memcpy(send_Buf, recv_Buf, sizeof(Send_datatype));
}

void ClientMain::returnBuffer(Send_datatype* buf)
{
	memcpy(buf, send_Buf, sizeof(Send_datatype));
}

SOCKET ClientMain::getClientSocket()
{
	return sockets;
}


// ObjectThread
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
