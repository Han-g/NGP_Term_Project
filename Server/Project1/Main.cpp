#include "Server.h"
#include "Common.h"

#define BUFSIZE 512
Send_datatype buf[BUFSIZE+1];
ServerMain* client;

DWORD WINAPI ObjectThread(LPVOID arg)
{
	client = static_cast<ServerMain*>(arg);

	while (1) {
		ResetEvent(InteractiveEvent);

		// 메시지 큐 확인
		if (client) {
			client->ProcessMessages();
		}

		server->ObjectCollision();
		server->KeyCheckClass();

		SetEvent(InteractiveEvent);
	}
	
	return 0;
}

DWORD WINAPI ClientThread(LPVOID arg)
{
	ServerMain* client = static_cast<ServerMain*>(arg);
	//int ClientNum = client->getClientNum();
	DWORD status;


	while (1)
	{
		int retval = recv(client->getClientSocket(), (char*)&buf, BUFSIZE, 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		
		// 클라이언트가 연결을 종료한 경우
		else if (retval == 0) {
			break;
		}

		// 읽기 완료 이벤트를 시그널 상태로 변경
		SetEvent(ClientRecvEvent[0]);

		// ObjectThread에서 데이터 처리 이벤트를 기다림
		WaitForSingleObject(ClientRecvEvent[1], INFINITE);

		// 메시지 구성
		EventQueue message;
		message.msgType = 1; // 예: 메시지 종류 식별을 위한 타입 설정

		// 메시지에 필요한 데이터 설정
		client->getBuffer(buf);			// 버퍼값 받아오기
		client->returnBuffer(buf);		// 처리 후 버퍼 저장
		client->EnqueueMsg(message);	// 메시지 큐에 메시지 추가

		// 클라이언트로 데이터 보내기 (원하는 처리에 따라 수정)
		send(client->getClientSocket(), (char*)buf, sizeof(Send_datatype), 0);
		WaitForSingleObject(InteractiveEvent, INFINITE);
	}

	return 0;
}

int main(void)
{
	//오류 확인을 위한 리턴 벨류 변수
	int retval; 

	//원속 버전 요청, 라이브러리(WS2_32.DLL) 초기화 
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) 
		return 1;

	// 소켓 생성이 잘 되었는지 오류 확인
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_display("socket()"); 

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));		//서버 addr 메모리 초기화
	serveraddr.sin_family = AF_INET;					// 소켓 주소체계
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);		//ip주소용 32비트 구조체
	serveraddr.sin_port = htons(nPort);					//포트번호를 네트워크 바이트 오더로 변경
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, 
		sizeof(serveraddr));							//로컬주소를 소켓에 연결 후 오류 처리를 위해 리턴벨류 삽입
	if (retval == SOCKET_ERROR) err_display("bind()");	// bind()가 제대로 되었는지 오류 확인

	// listen()
	retval = listen(listen_sock, SOMAXCONN);				// 연결대기 상태 제작후 오류 확인을 위한 리턴벨류 삽입
	if (retval == SOCKET_ERROR) err_display("listen()");	// 연결 대기상태의 오류 확인


	//서버용 스레드 생성
	server = new ObjectMain();

	// 이벤트 초기화
	ClientRecvEvent[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
	ClientRecvEvent[1] = CreateEvent(NULL, FALSE, FALSE, NULL);
	InteractiveEvent = CreateEvent(NULL, FALSE, TRUE, NULL);

	// 데이터 통신에 사용할 변수
	SOCKADDR_IN clientaddr;		//주소 받기용 구조체
	int addrlen;				// 주소 길이 확인을 위한 변수

	while (1) {
		// accept
		SOCKET client_sock;
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);

		//클라이언트 가득 찼으면 멈춤
		/*if (!server->ClientFullCheck()) {
			closesocket(client_sock);
			continue; 
		}*/

		//소켓타입이 INVALID_SOCKET으로 오류 반환시 알림
		if (client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
			addr, ntohs(clientaddr.sin_port));

		//스레드 생성
		HANDLE hThread;

		//클라이언트 초기화
		ServerMain* client = new ServerMain(client_sock, server->getClientNum());
		
		hThread = CreateThread(NULL, 0, ClientThread, client, 0, NULL);

		if (hThread == NULL) {
			delete client;
		}	
		else {
			CloseHandle(hThread);
		}	
		
		ServerMain* clientInstance = new ServerMain(INVALID_SOCKET, 0);
		//오브젝트 스레드 생성
		hThread = CreateThread(NULL, 0, ObjectThread, NULL, 0, NULL); 

		if (hThread == NULL) {
			cout << "오브젝트 스레드 생성 실패" << endl;
			return 0;
		}
		else {
			CloseHandle(hThread);
		}
	}


	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}