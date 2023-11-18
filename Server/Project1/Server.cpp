#include "Server.h"

ObjectMain* server;
HANDLE ClientRecvEvent[2];
HANDLE InteractiveEvent;

std::chrono::time_point<std::chrono::system_clock> gameTime;

// ServerThread
ServerMain::ServerMain(SOCKET recv_socket, int i)
{
	memset(recv_Buf, 0, sizeof(recv_Buf));
	memset(send_Buf, 0, sizeof(send_Buf));
	sockets = recv_socket;
	iClient = i;

	msgEventQueue = CreateEvent(NULL, FALSE, FALSE, NULL);
	gameStartTime = std::chrono::system_clock::now();
	gameTime = gameStartTime;
}

ServerMain::~ServerMain()
{
	CloseHandle(msgEventQueue);
}

void ServerMain::EnqueueMsg(const EventQueue& msg)
{
	// 메시지 큐에 메시지 추가
	SetEvent(msgEventQueue);
	messageQueue.push(msg);
}

void ServerMain::ProcessMessages()
{
	WaitForSingleObject(msgEventQueue, INFINITE);

	// 메시지 큐가 비어있지 않으면 처리
	while (!messageQueue.empty()) {
		EventQueue message = messageQueue.front();
		// 메시지 처리 로직 추가

		// 메시지 큐에서 메시지 제거
		messageQueue.pop();
	}
}

void ServerMain::getBuffer(Send_datatype* buf)
{
	memcpy(recv_Buf, &buf, sizeof(Send_datatype));
	memcpy(send_Buf, recv_Buf, sizeof(Send_datatype));
}

void ServerMain::returnBuffer(Send_datatype* buf)
{
	memcpy(buf, send_Buf, sizeof(Send_datatype));
}

double ServerMain::getTime()
{
	auto currentTime = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsedTime = currentTime - gameStartTime;

	return elapsedTime.count();
}

void ServerMain::GameStart()
{
	EventLoop();
}

void ServerMain::EventLoop()
{

}

SOCKET ServerMain::getClientSocket()
{
	return sockets;
}


// ObjectThread
ObjectMain::ObjectMain()
{
	ClientNum = 0;
	memset(recv_Buf, 0, sizeof(Send_datatype));
	memset(send_Buf, 0, sizeof(Send_datatype));
}

ObjectMain::~ObjectMain()
{

}

void ObjectMain::GameServer()
{
}

void ObjectMain::DisconnectClient(int iClient)
{
}

void ObjectMain::ObjectCollision()
{
}

void ObjectMain::updateTime(std::chrono::time_point<std::chrono::system_clock> gameTime)
{

}

void ObjectMain::KeyCheckClass()
{
}

bool ObjectMain::ClientFullCheck()
{
	return false;
}

bool ObjectMain::WaitEventCheck()
{
	return false;
}

int ObjectMain::getClientNum()
{
	return ClientNum++;
}
