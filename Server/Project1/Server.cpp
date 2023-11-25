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
	char_info = temp;

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
	recv_Buf = data;
	object_vector = data.object_info;
	wParam = data.wParam;
	events->check_obj(data);
}

Send_datatype ObjectMain::UpdateData()
{
	// object_vector 내 캐릭터 오브젝트 정보 추출
	auto index = find_if(object_vector.begin(), object_vector.end(), [](obj_info i) {
		return i.type == Char_Idle;
		});
	if (index != object_vector.end()) {
		char_info = *index;
	}
	else {
		std::cerr << "Can't Find Character info!" << std::endl;
		return recv_Buf;
	}

	// 캐릭터 오브젝트 내용 업데이트
	replace_if(object_vector.begin(), object_vector.end(), [](obj_info i) {	
		return i.type == Char_Idle;
		}, char_info);

	return send_Buf;
}



void ObjectMain::ObjectCollision()
{
	// 충돌 처리 구현
	// 플레이어 / 맵

	// 플레이어 / 버블

	// 버블 / 맵

}

void ObjectMain::updateTime(std::chrono::time_point<std::chrono::system_clock> gameTime)
{

}

void ObjectMain::KeyCheckClass()
{
	events->check_key();
	if (events->return_key_UP()) {
		ObjectCollision();
		char_info = events->update_char(0, 1);

	}
	if (events->return_key_DOWN()) {
		ObjectCollision();
		char_info = events->update_char(0, -1);

	}
	if (events->return_key_LEFT()) {
		ObjectCollision();
		char_info = events->update_char(-1, 0);

	}
	if (events->return_key_RIGHT()) {
		ObjectCollision();
		char_info = events->update_char(1, 0);

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
