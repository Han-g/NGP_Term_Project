#include "Server.h"
#include "Common.h"

#define BUFSIZE 51200

char buffer[BUFSIZE];
Send_datatype buf;
ServerMain* client; 

static ULONGLONG Frame = 10.0f;
static float ClientTime[2] = { 0.f, 0.f };
static float Time = 0.f;



void Serialize(Send_datatype* data, char* buf, size_t bufSize) {
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

void DeSerialize(Send_datatype* data, char* buf, size_t bufSize) {
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

/*
#include <cassert>
void test() {
	// �׽�Ʈ ������ ����
	Send_datatype originalData;
	originalData.wParam = 42;
	originalData.GameTime = 3.14;

	obj_info obj1 = { 1, 2, 3, 4, 5, 6, 7, 8 };
	obj_info obj2 = { 9, 10, 11, 12, 13, 14, 15, 16 };
	originalData.object_info.push_back(obj1);
	originalData.object_info.push_back(obj2);

	// ����ȭ �� ������ȭ
	const size_t bufferSize = sizeof(int) + sizeof(double) + originalData.object_info.size() * sizeof(obj_info);
	char buffer[BUFSIZE];
	Serialize(&originalData, buffer, bufferSize);

	Send_datatype deserializedData;
	DeSerialize(&deserializedData, buffer, bufferSize);

	// ���� �����Ϳ� ������ȭ�� ������ ��
	assert(originalData.wParam == deserializedData.wParam);
	assert(originalData.GameTime == deserializedData.GameTime);
	assert(originalData.object_info.size() == deserializedData.object_info.size());
	for (size_t i = 0; i < originalData.object_info.size(); ++i) {
		assert(originalData.object_info[i].posX == deserializedData.object_info[i].posX);
		assert(originalData.object_info[i].posY == deserializedData.object_info[i].posY);
		// ������ �ʵ忡 ���� �� �߰�
	}

	std::cout << "Serialization and deserialization successful!" << std::endl;

	return;
}
*/

DWORD WINAPI ObjectThread(LPVOID arg)
{
	client = static_cast<ServerMain*>(arg);

	while (1) {
		ResetEvent(InteractiveEvent);

		// �޽��� ť Ȯ��
		if (client) {
			client->ProcessMessages();
		}

		server->GameServer(buf);
		server->ObjectCollision();
		server->KeyCheckClass();

		SetEvent(InteractiveEvent);
		SetEvent(ClientRecvEvent[1]);
	}
	
	return 0;
}

DWORD WINAPI ClientThread(LPVOID arg)
{
	ServerMain* client = static_cast<ServerMain*>(arg);
	//int ClientNum = client->getClientNum();
	DWORD status;

	float fTime = 0.f;
	ULONGLONG StartTime = GetTickCount64();


	while (1)
	{
		if (GetTickCount64() - StartTime >= Frame)
		{
			fTime = GetTickCount64() - StartTime; // ���� �ð��� ���� ������ �ð� ���̷� �ð����
			fTime = fTime / 1000.0f; // ������ 1�ʿ� 60���� ����
			ClientTime[0] = ClientTime[1] = fTime; // Ŭ���̾�Ʈ ������ ����ȭ

			//fTime�� �ð����� �޴� �Լ� �߰�.

			StartTime = GetTickCount64();
		}

		int retval = recv(client->getClientSocket(), buffer, BUFSIZE, 0);
		DeSerialize(&buf, buffer, sizeof(char) * BUFSIZE);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		
		// Ŭ���̾�Ʈ�� ������ ������ ���
		else if (retval == 0) {
			break;
		}

		// �б� �Ϸ� �̺�Ʈ�� �ñ׳� ���·� ����
		SetEvent(ClientRecvEvent[0]);

		// ObjectThread���� ������ ó�� �̺�Ʈ�� ��ٸ�
		WaitForSingleObject(ClientRecvEvent[1], INFINITE);

		// �޽��� ����
		EventQueue message;
		message.msgType = 1; // ��: �޽��� ���� �ĺ��� ���� Ÿ�� ����

		// �޽����� �ʿ��� ������ ����
		client->getBuffer(buf);			// ���۰� �޾ƿ���
		client->returnBuffer(buf);		// ó�� �� ���� ����
		client->EnqueueMsg(message);	// �޽��� ť�� �޽��� �߰�

		// Ŭ���̾�Ʈ�� ������ ������ (���ϴ� ó���� ���� ����)
		Serialize(&buf, buffer, sizeof(char) * BUFSIZE);
		send(client->getClientSocket(), buffer, sizeof(char) * BUFSIZE, 0);

		WaitForSingleObject(InteractiveEvent, INFINITE);
	}

	return 0;
}

int main(void)
{
	//���� Ȯ���� ���� ���� ���� ����
	int retval;

	//���� ���� ��û, ���̺귯��(WS2_32.DLL) �ʱ�ȭ 
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) 
		return 1;

	// ���� ������ �� �Ǿ����� ���� Ȯ��
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_display("socket()"); 

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));		//���� addr �޸� �ʱ�ȭ
	serveraddr.sin_family = AF_INET;					// ���� �ּ�ü��
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);		//ip�ּҿ� 32��Ʈ ����ü
	serveraddr.sin_port = htons(nPort);					//��Ʈ��ȣ�� ��Ʈ��ũ ����Ʈ ������ ����
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, 
		sizeof(serveraddr));							//�����ּҸ� ���Ͽ� ���� �� ���� ó���� ���� ���Ϻ��� ����
	if (retval == SOCKET_ERROR) err_display("bind()");	// bind()�� ����� �Ǿ����� ���� Ȯ��

	// listen()
	retval = listen(listen_sock, SOMAXCONN);				// ������ ���� ������ ���� Ȯ���� ���� ���Ϻ��� ����
	if (retval == SOCKET_ERROR) err_display("listen()");	// ���� �������� ���� Ȯ��


	//������ ������ ����
	server = new ObjectMain();

	// �̺�Ʈ �ʱ�ȭ
	ClientRecvEvent[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
	ClientRecvEvent[1] = CreateEvent(NULL, FALSE, FALSE, NULL);
	InteractiveEvent = CreateEvent(NULL, FALSE, TRUE, NULL);

	// ������ ��ſ� ����� ����
	SOCKADDR_IN clientaddr;		//�ּ� �ޱ�� ����ü
	int addrlen;				// �ּ� ���� Ȯ���� ���� ����

	while (1) {
		// accept
		SOCKET client_sock;
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);

		//Ŭ���̾�Ʈ ���� á���� ����
		/*if (!server->ClientFullCheck()) {
			closesocket(client_sock);
			continue; 
		}*/

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

		//������ ����
		HANDLE hThread;

		//Ŭ���̾�Ʈ �ʱ�ȭ
		ServerMain* client = new ServerMain(client_sock, server->getClientNum());
		
		hThread = CreateThread(NULL, 0, ClientThread, client, 0, NULL);

		if (hThread == NULL) {
			delete client;
		}	
		else {
			CloseHandle(hThread);
		}	
		
		ServerMain* clientInstance = new ServerMain(INVALID_SOCKET, 0);
		//������Ʈ ������ ����
		hThread = CreateThread(NULL, 0, ObjectThread, NULL, 0, NULL); 

		if (hThread == NULL) {
			cout << "������Ʈ ������ ���� ����" << endl;
			return 0;
		}
		else {
			CloseHandle(hThread);
		}
	}


	// closesocket()
	closesocket(listen_sock);

	// ���� ����
	WSACleanup();
	return 0;
}