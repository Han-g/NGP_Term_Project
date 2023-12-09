#include "Common.h"
#include "ClientManager.h"
#include "ObjectManager.h"
//#include "Network.h"

char buffer[BUFSIZE + 1];

static ULONGLONG Frame = 10.0f;
static float ClientTime[2] = { 0.f, 0.f };
static float Time = 0.f;
std::atomic<int> frame(0);

//NetworkThread* networkThread;
std::mutex BufferMutex;
std::condition_variable ObjectThreadCV;
bool ObjectThreadReady[4] = { false, };
HANDLE ObjConnet[4];

struct ClientID {
	SOCKET socket;
	int clientID;
};
ClientID ID[4] = { NULL };

Send_datatype InputBuf, OutputBuf;
ServerData* serverData[4];
int nTotalSockets = 0;

// ���� ���� ���� �Լ�
bool AddSocketInfo(SOCKET sock);
void RemoveSocketInfo(int nIndex);

static void Serialize(Send_datatype* data, char* buf, size_t bufSize);
static void DeSerialize(Send_datatype* data, char* buf, size_t bufSize);

void ObjectSaver(DWORD clientID, const Send_datatype& data);
Send_datatype ObjectGetter(DWORD clientID);

void ObjectThread(int arg)
{
	int clientID = (int)(arg);
	ObjectManager* object = new ObjectManager();
	ObjConnet[clientID] = CreateEvent(NULL, FALSE, FALSE, NULL);

	while (1)
	{
		DWORD ret = WaitForSingleObject(ObjConnet[clientID], INFINITE);

		if (!ObjectThreadReady[clientID]) {
			ObjectThreadReady[clientID] = FALSE;
			break;
		}
		//std::cout << "Object Thread Section" << std::endl;

		Send_datatype buf = ObjectGetter(clientID);
		std::unique_lock<std::mutex> lock(BufferMutex);

		switch (ret)
		{
		case WAIT_TIMEOUT:
		case WAIT_FAILED:
			GetLastError();
			//lock.unlock();
			delete object;
			break;
		case WAIT_OBJECT_0:
			// bufferAccess �̺�Ʈ ���� �� ����	
			object->getClientID(clientID);
			object->GameSet(buf);
			if (buf.object_info.capacity() != 0) {
				std::cout << "CharPosition" << buf.object_info[225].posX << ", "
					<< buf.object_info[225].posY << "\r";
			}
			object->Key_Check();
			buf = object->Update();
			lock.unlock();
			ObjectSaver(clientID, buf);
			break;
		default:
			//lock.unlock();
			break;
		}

		ResetEvent(ObjConnet[clientID]);
	}

	delete object;
}

void ServerThread()
{
	//int frame = 0;
	float fTime = 0.f;
	ULONGLONG StartTime = GetTickCount64();
	ULONGLONG StartTime2 = GetTickCount64();

	while (1)
	{
		std::unique_lock<std::mutex> lock(BufferMutex);

		ULONGLONG gameTime = GetTickCount64();
		if (gameTime - StartTime >= Frame) {
			fTime = GetTickCount64() - StartTime;	// ���ð��� ���� ������ �ð� ���� �ð����
			fTime = fTime / 1000.0f;				// ������ 1�ʿ� 60���� ����
			ClientTime[0] = ClientTime[1] = fTime;	// Ŭ���̾�Ʈ ������ ����ȭ

			//fTime�� �ð����� �޴� �Լ� �߰�.
			frame++;
			StartTime = GetTickCount64();
		}

		if (gameTime - StartTime2 >= 1000)
		{
			//std::cout << "FPS : \r" << frame << std::endl;
			StartTime2 = GetTickCount64();
			frame = 0;
		}

		lock.unlock();
		// �����带 ���� �ð����� ����Ŵ (��: 16ms ��� = 60 FPS)
		//std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}

	return;
}

int main()
{
	int retval;
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 16;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) { return 1; }

	// ���� ����
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

	// �ͺ��ŷ ���� ��ȯ
	u_long on = 1;
	retval = ioctlsocket(listen_sock, FIONBIO, &on);
	if (retval == SOCKET_ERROR) { err_display("ioctlsocket()"); }

	std::thread objectThread[4];
	std::thread serverTime(ServerThread);

	SOCKET client_sock;
	int nready, addrlen;
	struct sockaddr_in clientaddr;

	while (1) {
		//std::cout << "Main Thread Section" << std::endl;
		// ���� �� �ʱ�ȭ
		fd_set rset, wset;
		FD_ZERO(&rset);
		FD_ZERO(&wset);
		FD_SET(listen_sock, &rset);
		for (int i = 0; i < nTotalSockets; i++) {
			if (serverData[i]->recvbytes > serverData[i]->sendbytes) {
				FD_SET(serverData[i]->socket, &wset);
			}
			else {
				FD_SET(serverData[i]->socket, &rset);
			}
		}

		// select()
		nready = select(nTotalSockets + 1, &rset, &wset, NULL, &timeout);
		if (nready == SOCKET_ERROR) { err_quit("select()"); }
		//std::cout << "select result: " << nready << std::endl;

		// ���� �� �˻�(1): Ŭ���̾�Ʈ ���� ����
		if (FD_ISSET(listen_sock, &rset)) {
			addrlen = sizeof(clientaddr);
			client_sock = accept(listen_sock,
				(struct sockaddr*)&clientaddr, &addrlen);
			if (client_sock == INVALID_SOCKET) {
				err_display("accept()");
				break;
			}
			else {
				// Ŭ���̾�Ʈ ���� ���
				char addr[INET_ADDRSTRLEN];
				inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
				printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
					addr, ntohs(clientaddr.sin_port));
				// ���� ���� �߰�: ���� �� ���� ����
				if (!AddSocketInfo(client_sock)) { 
					closesocket(client_sock); 
				}
				
				if (nTotalSockets > 0) {
					objectThread[nTotalSockets - 1] = std::thread(ObjectThread, nTotalSockets - 1);
					retval = send(client_sock, (char*)&ID[nTotalSockets - 1].clientID, sizeof(int), 0);
					serverData[nTotalSockets - 1]->recvbytes = retval;
					FD_SET(client_sock, &rset);
				}
			}
			if (--nready <= 0) { continue; }
		}

		// ���� �� �˻�(2): ������ ���
		for (int i = 0; i < nTotalSockets; i++) {
			std::unique_lock<std::mutex> lock(BufferMutex);
			ServerData* ptr = serverData[i];
			lock.unlock();

			if (FD_ISSET(ptr->socket, &rset)) {
				// ������ �ޱ�
				std::cout << "recive datas" << std::endl;
				retval = recv(ptr->socket, buffer, BUFSIZE, 0);

				if (retval == SOCKET_ERROR) {
					err_display("recv()");
					RemoveSocketInfo(i);
					SetEvent(ObjConnet[i]);
					objectThread[i].join();
					ResetEvent(ObjConnet[nTotalSockets - 1]);
					ObjConnet[nTotalSockets - 1] = nullptr;
				}
				else if (retval == 0) {
					RemoveSocketInfo(i);
					SetEvent(ObjConnet[i]);
					objectThread[i].join();
					ResetEvent(ObjConnet[nTotalSockets - 1]);
					ObjConnet[nTotalSockets - 1] = nullptr;
				}
				else {
					// Ŭ���̾�Ʈ ���� ���
					DeSerialize(&ptr->buf, buffer, BUFSIZE); 
					
					/*if (ptr->buf.object_info.capacity() >= 225) {
						std::cout << "recv Client : " << ptr->buf.wParam << std::endl;
					}*/

					ptr->recvbytes = retval;
					ptr->sendbytes = 0;
					std::unique_lock<std::mutex> lock(BufferMutex);
					serverData[i] = ptr;
					lock.unlock();

					addrlen = sizeof(clientaddr);
					getpeername(ptr->socket, (struct sockaddr*)&clientaddr, &addrlen);
					char addr[INET_ADDRSTRLEN];
					inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
				}
			}
			else if (FD_ISSET(ptr->socket, &wset)) {
				SetEvent(ObjConnet[i]);
				std::cout << "send datas" << std::endl;

				retval = send(ptr->socket, (char*)&nTotalSockets, sizeof(int), 0);
				if (retval == SOCKET_ERROR) {
					err_display("send()");
				}

				for (int j = 0; j < 4; ++j) {
					if (j < nTotalSockets) {
						Serialize(&serverData[j]->buf, buffer, BUFSIZE);
						retval = send(ptr->socket, buffer + serverData[j]->sendbytes,
							serverData[j]->recvbytes - serverData[j]->sendbytes, 0);
						
						//std::cout << i+1 << " Client send : " << serverData[i]->buf.wParam << std::endl;
						
						if (retval == SOCKET_ERROR) {
							err_display("send()");
						}
						else {
							std::unique_lock<std::mutex> lock(BufferMutex);
							serverData[j]->sendbytes += retval;
							if (serverData[j]->recvbytes == serverData[j]->sendbytes) {
								serverData[j]->recvbytes = serverData[j]->sendbytes = 0;
							}
							lock.unlock();
						}
					}
					else {
						std::cout << "select set check" << std::endl;
					}
				}
				//ptr->recvbytes = 0;
			}
		}
	}

	serverTime.join();
	return 0;
}

// ���� ���� �߰�
bool AddSocketInfo(SOCKET sock)
{
	if (nTotalSockets >= 4) {
		printf("[����] ���� ������ �߰��� �� �����ϴ�!\n");
		return false;
	}
	ServerData* ptr = new ServerData;
	if (ptr == NULL) {
		printf("[����] �޸𸮰� �����մϴ�!\n");
		delete ptr;
		return false;
	}

	ptr->socket = sock;
	ptr->recvbytes = 0;
	ptr->sendbytes = 0;

	for (int i = 0; i < 4; ++i) {
		if (serverData[i] == NULL) {
			std::unique_lock<std::mutex> lock(BufferMutex);
			serverData[i] = ptr;
			lock.unlock();
			ObjectThreadReady[i] = true;
			ID[i].socket = sock;
			ID[i].clientID = i;
			break;
		}
		if (i == 3) {
			printf("[����] ���� ������ �߰��� �� �����ϴ�!\n");
			return false;
		}
	}
	nTotalSockets++;

	return true;
}

// ���� ���� ����
void RemoveSocketInfo(int nIndex)
{
	ServerData* ptr = serverData[nIndex];

	// Ŭ���̾�Ʈ ���� ���
	struct sockaddr_in clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(ptr->socket, (struct sockaddr*)&clientaddr, &addrlen);

	// Ŭ���̾�Ʈ ���� ���
	char addr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
	printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		addr, ntohs(clientaddr.sin_port));

	// ���� �ݱ�
	closesocket(ptr->socket);
	delete ptr;

	if (nIndex != (nTotalSockets - 1)) {
		for (int i = nIndex; i < 3; ++i) {
			std::unique_lock<std::mutex> lock(BufferMutex);
			serverData[i] = serverData[i + 1];
			lock.unlock();
		}
		std::unique_lock<std::mutex> lock(BufferMutex);
		serverData[nTotalSockets - 1] = NULL;
		lock.unlock();
	}
	else {
		std::unique_lock<std::mutex> lock(BufferMutex);
		serverData[nTotalSockets - 1] = NULL;
		lock.unlock();
	}
	ObjectThreadReady[nTotalSockets - 1] = false;

	--nTotalSockets;
}

static void Serialize(Send_datatype* data, char* buf, size_t bufSize) {
	// ������ ũ�� Ȯ��
	size_t dataSize = sizeof(int) + sizeof(double) + data->object_info.capacity() * sizeof(obj_info);

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

	std::memcpy(buf, data->object_info.data(), data->object_info.capacity() * sizeof(obj_info));
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
	std::unique_lock<std::mutex> lock(BufferMutex);
	if (data.object_info.capacity() != 0) {
		serverData[(int)clientID]->buf.GameTime = data.GameTime;
		serverData[(int)clientID]->buf.wParam = data.wParam;
		try
		{
			if (serverData[(int)clientID]->buf.object_info.empty()) {
				serverData[(int)clientID]->buf.object_info = data.object_info;
			}
			else {
				serverData[(int)clientID]->buf.object_info.clear();
				serverData[(int)clientID]->buf.object_info = data.object_info;
			}
		}
		catch (const std::out_of_range& e)
		{
			std::cerr << "Out of range exception: " << e.what() << std::endl;
		}
	}
	lock.unlock();
}
Send_datatype ObjectGetter(DWORD clientID)
{
	Send_datatype copy;
	std::unique_lock<std::mutex> lock(BufferMutex); 
	try {
		copy = serverData[(int)clientID]->buf;
	}
	catch (const std::out_of_range& e) {
		std::cerr << "Out of range exception: " << e.what() << std::endl;
	}
	lock.unlock();
	return copy;
}