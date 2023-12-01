#include "Common.h"
#include "ClientManager.h"
#include "ObjectManager.h"

#define BUFSIZE 50000

char buffer[BUFSIZE + 1];

struct ServerData
{
	SOCKET socket;
	Send_datatype buf;
	int sendbytes, recvbytes;
};

ServerData* serverData[4];
int nTotalSockets = 0;

// 소켓 정보 관리 함수
bool AddSocketInfo(SOCKET sock);
void RemoveSocketInfo(int nIndex);

static void Serialize(Send_datatype* data, char* buf, size_t bufSize);
static void DeSerialize(Send_datatype* data, char* buf, size_t bufSize);

void ObjectSaver(DWORD clientID, const Send_datatype& data);
Send_datatype ObjectGetter(DWORD clientID);

static ULONGLONG Frame = 10.0f;
static float ClientTime[2] = { 0.f, 0.f };
static float Time = 0.f;

std::vector<ClientInfo> clients;
std::vector<Send_datatype> Server_bufVector;
Send_datatype Server_bufs;
Send_datatype Server_bufArray[4];

// Event 선언
HANDLE bufferAccess;
HANDLE UpdateMutex;


int main()
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) { return 1; }

	// 소켓 생성
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

	// 넌블로킹 소켓 전환
	u_long on = 1;
	retval = ioctlsocket(listen_sock, FIONBIO, &on);
	if (retval == SOCKET_ERROR) { err_display("ioctlsocket()"); }

	fd_set rset, wset;
	int nready;
	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	int addrlen;

	while (1) {
		// 소켓 셋 초기화
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
		nready = select(0, &rset, &wset, NULL, NULL);
		if (nready == SOCKET_ERROR) err_quit("select()");

		// 소켓 셋 검사(1): 클라이언트 접속 수용
		if (FD_ISSET(listen_sock, &rset)) {
			addrlen = sizeof(clientaddr);
			client_sock = accept(listen_sock,
				(struct sockaddr*)&clientaddr, &addrlen);
			if (client_sock == INVALID_SOCKET) {
				err_display("accept()");
				break;
			}
			else {
				// 클라이언트 정보 출력
				char addr[INET_ADDRSTRLEN];
				inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
				printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
					addr, ntohs(clientaddr.sin_port));
				// 소켓 정보 추가: 실패 시 소켓 닫음
				if (!AddSocketInfo(client_sock)) { closesocket(client_sock); }
			}
			if (--nready <= 0) { continue; }
		}

		// 소켓 셋 검사(2): 데이터 통신
		for (int i = 0; i < nTotalSockets; i++) {
			ServerData* ptr = serverData[i];

			if (FD_ISSET(ptr->socket, &rset)) {
				// 데이터 받기
				retval = recv(ptr->socket, buffer, BUFSIZE, 0);
				if (retval == SOCKET_ERROR) {
					err_display("recv()");
					RemoveSocketInfo(i);
				}
				else if (retval == 0) {
					RemoveSocketInfo(i);
				}
				else {
					// 클라이언트 정보 얻기
					DeSerialize(&ptr->buf, buffer, BUFSIZE);
					ptr->recvbytes = retval;
					ptr->sendbytes = 0;
					serverData[i] = ptr;

					addrlen = sizeof(clientaddr);
					getpeername(ptr->socket, (struct sockaddr*)&clientaddr, &addrlen);
					char addr[INET_ADDRSTRLEN];
					inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
				}
			}
			else if (FD_ISSET(ptr->socket, &wset)) {
				// 데이터 보내기
				Serialize(&ptr->buf, buffer, BUFSIZE);
				retval = send(ptr->socket, buffer + ptr->sendbytes,
					ptr->recvbytes - ptr->sendbytes, 0);
				if (retval == SOCKET_ERROR) {
					err_display("send()");
					RemoveSocketInfo(i);
				}
				else {
					ptr->sendbytes += retval;
					if (ptr->recvbytes == ptr->sendbytes) {
						//ptr->recvbytes = 
						ptr->sendbytes = 0;
					}
				}

				for (int i = 0; i < 4; ++i) {
					if (i < nTotalSockets) {
						Serialize(&serverData[i]->buf, buffer, BUFSIZE);
						retval = send(ptr->socket, buffer + serverData[i]->sendbytes,
							serverData[i]->recvbytes - serverData[i]->sendbytes, 0);
						if (retval == SOCKET_ERROR) {
							err_display("send()");
							RemoveSocketInfo(i);
						}
						else {
							serverData[i]->sendbytes += retval;
							if (serverData[i]->recvbytes == serverData[i]->sendbytes) {
								serverData[i]->recvbytes = serverData[i]->sendbytes = 0;
							}
						}
					}
					else {
						char* tem = new char[BUFSIZE];
						memset(tem, 0, BUFSIZE);
						retval = send(ptr->socket, tem, BUFSIZE, 0);
					}
				}

				ptr->recvbytes = 0;
			}
		}
	}
}

// 소켓 정보 추가
bool AddSocketInfo(SOCKET sock)
{
	if (nTotalSockets >= 4) {
		printf("[오류] 소켓 정보를 추가할 수 없습니다!\n");
		return false;
	}
	ServerData* ptr = new ServerData;
	if (ptr == NULL) {
		printf("[오류] 메모리가 부족합니다!\n");
		return false;
	}
	ptr->socket = sock;
	ptr->recvbytes = 0;
	ptr->sendbytes = 0;
	serverData[nTotalSockets++] = ptr;
	return true;
}

// 소켓 정보 삭제
void RemoveSocketInfo(int nIndex)
{
	ServerData* ptr = serverData[nIndex];

	// 클라이언트 정보 얻기
	struct sockaddr_in clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(ptr->socket, (struct sockaddr*)&clientaddr, &addrlen);

	// 클라이언트 정보 출력
	char addr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		addr, ntohs(clientaddr.sin_port));

	// 소켓 닫기
	closesocket(ptr->socket);
	delete ptr;

	if (nIndex != (nTotalSockets - 1)) { serverData[nIndex] = serverData[nTotalSockets - 1]; }
	--nTotalSockets;
}

DWORD WINAPI ObjectThread(LPVOID arg)
{
	ClientInfo* clientInfo = static_cast<ClientInfo*>(arg);
	ObjectManager* object = new ObjectManager();

	while (1)
	{
		// bufferAccess 이벤트 설정 시 수행
		object->GameSet(ObjectGetter(clientInfo->clientID));
		WaitForSingleObject(clientInfo->clientEvent, INFINITE);
		object->Key_Check();
		//ObjectSaver(clientInfo->clientID, Server_bufVector[(int)clientInfo->clientID-1]);
		ObjectSaver(clientInfo->clientID, Server_bufArray[(int)clientInfo->clientID - 1]);

		ResetEvent(clientInfo->clientEvent);
		// send 수행 이벤트
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
			fTime = GetTickCount64() - StartTime;	// 현시간과 이전 프레임 시간 차로 시간계산
			fTime = fTime / 1000.0f;				// 프레임 1초에 60으로 고정
			ClientTime[0] = ClientTime[1] = fTime;	// 클라이언트 프레임 동기화

			//fTime의 시간값을 받는 함수 추가.
			frame++;
			StartTime = GetTickCount64();
		}

		if (GetTickCount64() - StartTime >= 1000)
		{
			std::cout << "FPS : " << frame << std::endl;
			StartTime2 = GetTickCount64();
			frame = 0;
		}

		// 클라이언트에게서 오브젝트 정보 받아오기
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
			// 모든 클라이언트 오브젝트 관리
			if (Server_bufArray[(int)clientInfo->clientID - 1].wParam != 0) {
				SetEvent(clientInfo->clientEvent);
			}

			//server->getBuffer(ObjectGetter(clientInfo->clientID));
			//ObjectSaver(clientInfo->clientID, server->returnBuffer());
		}

		// 배정된 ClientThread의 클라이언트 정보 전달
		for (auto& clientVector : clients) {
			if (clientVector.clientID == clientInfo->clientID) {
				Serialize(&Server_bufs, buffer, sizeof(char) * BUFSIZE);
				send(clientVector.socket, buffer, sizeof(char) * BUFSIZE, 0);
			}
		}
		// 모든 클라이언트 정보 전달
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

static void Serialize(Send_datatype* data, char* buf, size_t bufSize) {
	// 데이터 크기 확인
	size_t dataSize = sizeof(int) + sizeof(double) + data->object_info.size() * sizeof(obj_info);

	// 버퍼 크기 확인
	if (bufSize < dataSize) {
		std::cerr << "Buffer size is too small for serialization!" << std::endl;
		return;
	}

	// 버퍼 초기화
	memset(buf, 0, dataSize);

	// 데이터 복사
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

	// 버퍼 초기화
	data->object_info.clear();
	data->GameTime = 0.0f;
	data->wParam = 0;

	// 데이터 복사
	std::memcpy(&data->wParam, buf, sizeof(int));
	buf += sizeof(int);

	std::memcpy(&data->GameTime, buf, sizeof(double));
	buf += sizeof(double);

	// obj_info 역직렬화
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