#include "Server.h"

ObjectMain* server;
HANDLE ClientRecvEvent[2];
HANDLE InteractiveEvent;

std::chrono::time_point<std::chrono::system_clock> gameTime;

// ServerThread
ServerMain::ServerMain(SOCKET recv_socket, int i)
{
	obj_info temp = { 0,0,0,0,0,0,{0,0} };
	recv_Buf.object_info.push_back(temp);
	recv_Buf.GameTime = 0;
	recv_Buf.wParam = 0;

	send_Buf.object_info.push_back(temp);
	send_Buf.GameTime = 0;
	send_Buf.wParam = 0;

	sockets = recv_socket;
	numofclient = i;

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
		switch (message.msgType) {
		case 1:
			break;
		default:
			break;
		}
		// 메시지 큐에서 메시지 제거
		messageQueue.pop();
	}
}

int ServerMain::getClientNum()
{
	return numofclient;
}

void ServerMain::getBuffer(Send_datatype buf)
{
	//memcpy(recv_Buf, &buf, sizeof(Send_datatype));
	//memcpy(send_Buf, recv_Buf, sizeof(Send_datatype));
	recv_Buf = buf;
	send_Buf = recv_Buf;
}

void ServerMain::returnBuffer(Send_datatype buf)
{
	//memcpy(buf, send_Buf, sizeof(Send_datatype));
	buf = send_Buf;
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

void ServerMain::DisconnectClient(int clientnum)
{
	numofclient -= 1;


	if (numofclient < 0)
		numofclient = 0;
}

SOCKET ServerMain::getClientSocket()
{
	return sockets;
}


// ObjectThread
ObjectMain::ObjectMain()
{
	obj_info temp = { 0,0,0,0,0,0,{0,0} };
	recv_Buf.object_info.push_back(temp);
	recv_Buf.GameTime = 0;
	recv_Buf.wParam = 0;

	send_Buf.object_info.push_back(temp);
	send_Buf.GameTime = 0;
	send_Buf.wParam = 0;

	m_Key_UP	= FALSE;
	m_Key_DOWN	= FALSE;
	m_Key_LEFT	= FALSE;
	m_Key_RIGHT = FALSE;
	m_Key_BUBBLE = FALSE;
	m_Key_ITEM	= FALSE;

	events = new EventHandle();
	wParam = 0;
	bubble_count = 0;
	ClientNum = 0;
}

ObjectMain::~ObjectMain()
{
	delete events;
}

void ObjectMain::GameServer(Send_datatype data)
{
	//memcpy(recv_Buf, data, sizeof(Send_datatype));
	object_vector = data.object_info;
	wParam = data.wParam;
	events->check_obj(data);
}



void ObjectMain::ObjectCollision()
{
}

void ObjectMain::updateTime(std::chrono::time_point<std::chrono::system_clock> gameTime)
{

}

void ObjectMain::KeyCheckClass()
{
	events->check_key();
	if (events->return_key_UP()) {
		ObjectCollision();
		//events->update_char();

	}
	if (events->return_key_DOWN()) {
		ObjectCollision();

	}
	if (events->return_key_LEFT()) {
		ObjectCollision();

	}
	if (events->return_key_RIGHT()) {
		ObjectCollision();

	}
	if (events->return_key_BUBBLE()) {
		ObjectCollision();

	}
	if (events->return_key_ITEM()) {
		ObjectCollision();

	}
}

bool ObjectMain::ClientFullCheck()
{
	return ClientNum < 5;
}

bool ObjectMain::WaitEventCheck()
{
	return false;
}

int ObjectMain::getClientNum()
{
	return ClientNum++;
}
