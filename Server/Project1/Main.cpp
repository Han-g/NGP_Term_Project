#include "Common.h"
#include "ClientManager.h"
#include "ObjectManager.h"

#define BUFSIZE 50000

char buffer[BUFSIZE];
Send_datatype Server_bufs;
//ClientManager* client;
//ObjectManager* object;

static ULONGLONG Frame = 10.0f;
static float ClientTime[2] = { 0.f, 0.f };
static float Time = 0.f;
std::vector<ClientInfo> clients;
std::vector<Send_datatype> Server_bufVector;
Send_datatype Server_bufArray[4];

// Event ����
HANDLE bufferAccess;
HANDLE UpdateMutex;

static void Serialize(Send_datatype* data, char* buf, size_t bufSize) {
	// ������ ũ�� Ȯ��
	size_t dataSize = sizeof(int) + sizeof(double) + data->object_info.size() * sizeof(obj_info);

	// ���� ũ�� Ȯ��
	if (bufSize < dataSize) {
		std::cerr << "Buffer size is too small for serialization!" << std::endl;
		return;
	}

	// ���� �ʱ�ȭ
	memset(buf, 0, dataSize);

	// ������ ����
	std::memcpy(buf, &data->wParam, sizeof(int));
	buf += sizeof(int);

	std::memcpy(buf, &data->GameTime, sizeof(double));
	buf += sizeof(double);

	std::memcpy(buf, data->object_info.data(), data->object_info.size() * sizeof(obj_info));
}

static void DeSerialize(Send_datatype* data, char* buf, size_t bufSize) {
	if (bufSize < sizeof(int) + sizeof(double)) {
		std::cerr << "Buffer size is too small for deserialization!" << std::endl;
		return;
	}

	// ���� �ʱ�ȭ
	data->object_info.clear();
	data->GameTime = 0.0f;
	data->wParam = 0;

	// ������ ����
	std::memcpy(&data->wParam, buf, sizeof(int));
	buf += sizeof(int);

	std::memcpy(&data->GameTime, buf, sizeof(double));
	buf += sizeof(double);

	// obj_info ������ȭ
	size_t objInfoSize = (bufSize - sizeof(int) - sizeof(double)) / sizeof(obj_info);
	data->object_info.resize(objInfoSize);
	std::memcpy(data->object_info.data(), buf, objInfoSize * sizeof(obj_info));
}

void ObjectSaver(DWORD clientID, const Send_datatype& data)
{
	WaitForSingleObject(bufferAccess, INFINITE);
	/*if (Server_bufVector.empty()) {
		Server_bufVector.push_back(data);
	}
	else {
		Server_bufVector[(int)clientID-1] = data;
	}*/
	Server_bufArray[(int)clientID - 1].GameTime = data.GameTime;
	Server_bufArray[(int)clientID - 1].wParam = data.wParam;
	if (Server_bufArray[(int)clientID - 1].object_info.empty()) {
		Server_bufArray[(int)clientID - 1].object_info = data.object_info;
	}
	else {
		Server_bufArray[(int)clientID - 1].object_info.clear();
		Server_bufArray[(int)clientID - 1].object_info = data.object_info;
	}
	ReleaseMutex(bufferAccess);
}
Send_datatype ObjectGetter(DWORD clientID)
{
	WaitForSingleObject(bufferAccess, INFINITE);
	Send_datatype copy;
	/*if (Server_bufVector.size() > 0) {
		copy = Server_bufVector[(int)clientID-1];
	}*/
	copy = Server_bufArray[(int)clientID - 1];
	ReleaseMutex(bufferAccess);
	return copy;
}

DWORD WINAPI ObjectThread(LPVOID arg)
{
	ClientInfo* clientInfo = static_cast<ClientInfo*>(arg);
	ObjectManager* object = new ObjectManager();

	while (1)
	{
		// bufferAccess �̺�Ʈ ���� �� ����
		object->GameSet(ObjectGetter(clientInfo->clientID));
		WaitForSingleObject(clientInfo->clientEvent, INFINITE);
		object->Key_Check();
		//ObjectSaver(clientInfo->clientID, Server_bufVector[(int)clientInfo->clientID-1]);
		ObjectSaver(clientInfo->clientID, Server_bufArray[(int)clientInfo->clientID - 1]);

		ResetEvent(clientInfo->clientEvent);
		// send ���� �̺�Ʈ
		if (clients[clientInfo->clientID - 1].socket == 0) {
			break;
		}
	}

	delete object;
	return 0;
}


DWORD WINAPI ServerThread(LPVOID arg)
{
	ClientInfo* clientInfo = static_cast<ClientInfo*>(arg);
	ClientManager* server = new ClientManager(clientInfo->socket, clientInfo->clientID);

	int frame = 0;
	float fTime = 0.f;
	ULONGLONG StartTime = GetTickCount64();
	ULONGLONG StartTime2 = GetTickCount64();

	while (1)
	{
		//WaitForSingleObject(clientInfo->clientEvent, INFINITE);
		if (GetTickCount64() - StartTime >= Frame) {
			fTime = GetTickCount64() - StartTime;	// ���ð��� ���� ������ �ð� ���� �ð����
			fTime = fTime / 1000.0f;				// ������ 1�ʿ� 60���� ����
			ClientTime[0] = ClientTime[1] = fTime;	// Ŭ���̾�Ʈ ������ ����ȭ

			//fTime�� �ð����� �޴� �Լ� �߰�.
			frame++;
			StartTime = GetTickCount64();
		}

		if (GetTickCount64() - StartTime >= 1000)
		{
			std::cout << "FPS : " << frame << std::endl;
			StartTime2 = GetTickCount64();
			frame = 0;
		}

		// Ŭ���̾�Ʈ���Լ� ������Ʈ ���� �޾ƿ���
		int retval = recv(clientInfo->socket, buffer, sizeof(char) * BUFSIZE, 0);
		if (retval == SOCKET_ERROR || retval == 0) {
			err_display("recv()");
			clients[(int)clientInfo->clientID - 1].socket = 0;
			memset(&Server_bufArray[(int)clientInfo->clientID - 1], 0, sizeof(Send_datatype));
			break;
		}
		else {
			//Server_bufs = ObjectGetter(clientInfo->clientID);
			DeSerialize(&Server_bufs, buffer, sizeof(char) * BUFSIZE);
			ObjectSaver(clientInfo->clientID, Server_bufs);
			// ��� Ŭ���̾�Ʈ ������Ʈ ����
			if (Server_bufArray[(int)clientInfo->clientID - 1].wParam != 0) {
				SetEvent(clientInfo->clientEvent);
			}

			//server->getBuffer(ObjectGetter(clientInfo->clientID));
			//ObjectSaver(clientInfo->clientID, server->returnBuffer());
		}

		// ������ ClientThread�� Ŭ���̾�Ʈ ���� ����
		for (auto& clientVector : clients) {
			if (clientVector.clientID == clientInfo->clientID) {
				Serialize(&Server_bufs, buffer, sizeof(char) * BUFSIZE);
				send(clientVector.socket, buffer, sizeof(char) * BUFSIZE, 0);
			}
		}
		// ��� Ŭ���̾�Ʈ ���� ����
		for (auto& clientVector : clients) {
			Serialize(&Server_bufs, buffer, sizeof(char) * BUFSIZE);
			send(clientVector.socket, buffer, sizeof(char) * BUFSIZE, 0);
		}

		/*Serialize(&Server_bufs, buffer, sizeof(char) * BUFSIZE);
		send(server->getClientSocket(), buffer, sizeof(char) * BUFSIZE, 0);*/
		ResetEvent(clientInfo->clientEvent);
	}

	delete server;
	return 0;
}

int main()
{
	int retval;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		return 1;
	}

	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) { err_display("socket()"); }

	// bind
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(nPort);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_display("bind()");

	// listen
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) { err_display("listen()"); }

	// WSAAsyncSelect
	//retval = WSAAsyncSelect(listen_sock, )

	// ��ſ� ����� ����
	SOCKADDR_IN clientaddr;
	int addrlen;
	int client_count = 0;
	ClientInfo newClient;

	// ���� ����
	clients.reserve(4);
	ClientInfo tempClient = { 0, 0, 0 };
	clients.assign(4, tempClient);
	//Server_bufVector.reserve(4);
	memset(Server_bufArray, 0, sizeof(Send_datatype) * 4);

	// �̺�Ʈ �ʱ�ȭ
	bufferAccess = CreateMutex(NULL, FALSE, NULL);
	UpdateMutex = CreateMutex(NULL, FALSE, NULL);

	while (1) {
		SOCKET client_sock;
		// accept
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);

		//
		// ���� Ŭ���̾�Ʈ ���� �ڵ� ����
		//

		//����Ÿ���� INVALID_SOCKET���� ���� ��ȯ�� �˸�
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		// ������ Ŭ���̾�Ʈ ���� ���
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			addr, ntohs(clientaddr.sin_port));

		// ���� Ŭ���̾�Ʈ ID ����
		if (client_count + 1 > 4) {
			for (auto& info : clients) {
				if (info.socket == 0) {
					client_count = (int)info.clientID;
					break;
				}
			}
			closesocket(client_sock);
			continue;
		}

		newClient.socket = client_sock;
		newClient.clientID = ++client_count;
		newClient.clientEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		clients[newClient.clientID - 1] = newClient;

		//������ ����
		HANDLE hThread;
		hThread = CreateThread(NULL, 0, ServerThread, &clients[newClient.clientID - 1], 0, NULL);
		if (hThread == NULL) {
			std::cerr << "Cant create Client Thread!" << std::endl;
		}
		else {
			clients[newClient.clientID - 1].clientEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			hThread = CreateThread(NULL, 0, ObjectThread, &clients[newClient.clientID - 1], 0, NULL);
			if (hThread == NULL) {
				std::cerr << "ObjectThreading Failed!" << std::endl;
				return 0;
			}
			else { CloseHandle(hThread); }
		}
	}

	// ���� ����
	WSACleanup();
	return 0;
}