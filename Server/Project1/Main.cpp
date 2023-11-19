#include "Server.h"

int main(void)
{
	int retval; //���� Ȯ���� ���� ���� ���� ����

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) //���� ���� ��û, ���̺귯��(WS2_32.DLL) �ʱ�ȭ 
		return 1;


	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_display("socket()"); // ���� ������ �� �Ǿ����� ���� Ȯ��

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr)); //���� addr �޸� �ʱ�ȭ
	serveraddr.sin_family = AF_INET; // ���� �ּ�ü��
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); //ip�ּҿ� 32��Ʈ ����ü
	serveraddr.sin_port = htons(nPort); //��Ʈ��ȣ�� ��Ʈ��ũ ����Ʈ ������ ����
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr)); //�����ּҸ� ���Ͽ� ���� �� ���� ó���� ���� ���Ϻ��� ����
	if (retval == SOCKET_ERROR) err_display("bind()"); // bind()�� ����� �Ǿ����� ���� Ȯ��

	// listen()
	retval = listen(listen_sock, SOMAXCONN); // ������ ���� ������ ���� Ȯ���� ���� ���Ϻ��� ����
	if (retval == SOCKET_ERROR) err_display("listen()"); // ���� �������� ���� Ȯ��

	//������Ʈ ������ ����
	HANDLE hThread; 
	server = new ServerMain(); //������ ������ ����

	hThread = CreateThread(NULL, 0, ObjectThread, NULL, 0, NULL); //������ ����
	if (hThread == NULL) {
		cout << "������Ʈ ������ ���� ����" << endl;
		return 0;
	}
	//������ ���� �� �ȵǾ��� ��� ���� �޼��� �߼�
	else {
		CloseHandle(hThread);
	}
	//������ �� �Ǿ����� �ڵ� �ݱ�

	// ������ ��ſ� ����� ����

	SOCKADDR_IN clientaddr; //�ּ� �ޱ�� ����ü
	int addrlen; // �ּ� ���� Ȯ���� ���� ����

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
		//����Ÿ���� INVALID_SOCKET���� ���� ��ȯ�� �˸�

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