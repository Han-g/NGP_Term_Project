#include "Server.h"

int main(void)
{
	int retval; //오류 확인을 위한 리턴 벨류 변수

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) //원속 버전 요청, 라이브러리(WS2_32.DLL) 초기화 
		return 1;


	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_display("socket()"); // 소켓 생성이 잘 되었는지 오류 확인

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr)); //서버 addr 메모리 초기화
	serveraddr.sin_family = AF_INET; // 소켓 주소체계
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); //ip주소용 32비트 구조체
	serveraddr.sin_port = htons(nPort); //포트번호를 네트워크 바이트 오더로 변경
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr)); //로컬주소를 소켓에 연결 후 오류 처리를 위해 리턴벨류 삽입
	if (retval == SOCKET_ERROR) err_display("bind()"); // bind()가 제대로 되었는지 오류 확인

	// listen()
	retval = listen(listen_sock, SOMAXCONN); // 연결대기 상태 제작후 오류 확인을 위한 리턴벨류 삽입
	if (retval == SOCKET_ERROR) err_display("listen()"); // 연결 대기상태의 오류 확인

	//오브젝트 쓰레드 생성
	HANDLE hThread; 
	server = new ServerMain(); //서버용 스레드 생성

	hThread = CreateThread(NULL, 0, ObjectThread, NULL, 0, NULL); //스레드 생성
	if (hThread == NULL) {
		cout << "오브젝트 스레드 생성 실패" << endl;
		return 0;
	}
	//쓰레드 생성 이 안되었을 경우 오류 메세지 발송
	else {
		CloseHandle(hThread);
	}
	//생성이 잘 되었으면 핸들 닫기

	// 데이터 통신에 사용할 변수

	SOCKADDR_IN clientaddr; //주소 받기용 구조체
	int addrlen; // 주소 길이 확인을 위한 변수

	while (1) {
		SOCKET* client_sock = new SOCKET();
		addrlen = sizeof(clientaddr);
		*client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);

		if (!server->ClientFullCheck()) {
			closesocket(*client_sock);
			continue; 
		}
		//클라이언트 가득 찼으면 멈춤

		if (*client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}
		//소켓타입이 INVALID_SOCKET으로 오류 반환시 알림

		// 접속한 클라이언트 정보 출력
		printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		//클라이언트 초기화
		ClientMain* client = new ClientMain(client_sock, server->ClientNum());

		hThread = CreateThread(NULL, 0, ClientThread, client, 0, NULL);

		if (hThread == NULL) {

			delete client;
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