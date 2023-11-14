#include "Server.h"

int main(void)
{
	int retval;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_display("socket()");

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(9000);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_display("bind()");

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_display("listen()");

	//������Ʈ ������ ����
	HANDLE hThread;
	server = new ServerMain();

	hThread = CreateThread(NULL, 0, ObjectThread, NULL, 0, NULL);
	if (hThread == NULL) {
		cout << "������Ʈ ������ ���� ����" << endl;
		return 0;
	}
	else {
		CloseHandle(hThread);
	}

	// ������ ��ſ� ����� ����

	SOCKADDR_IN clientaddr;
	int addrlen;

	while (1) {
		SOCKET* client_sock = new SOCKET();
		addrlen = sizeof(clientaddr);
		*client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);

		if (!server->ClientFullCheck()) {
			closesocket(*client_sock);
			continue; 
		}
		//Ŭ���̾�Ʈ ���� á���� ����

		if (*client_sock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}
		// ������ Ŭ���̾�Ʈ ���� ���

		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		//Ŭ���̾�Ʈ �ʱ�ȭ
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

	// ���� ����
	WSACleanup();
	return 0;
}