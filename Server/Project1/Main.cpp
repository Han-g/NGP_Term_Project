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

		// �޽��� ť Ȯ��
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
		send(client->getClientSocket(), (char*)buf, sizeof(Send_datatype), 0);
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