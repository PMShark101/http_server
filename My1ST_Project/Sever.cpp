#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<Winsock2.h>

using namespace std;

void handleHttpRequest(SOCKET clientSocket) {
	char recvBuffer[4096];
	int recvLength = recv(clientSocket, recvBuffer, 4096, 0);
	if (recvLength <= 0) {
		return;
	}

	// �����յ�����ת��Ϊ�ַ���
	string request(recvBuffer, recvLength);

	// ���� HTTP ������
	stringstream ss(request);
	string method, uri, version;
	ss >> method >> uri >> version;

	if (method == "GET") {
		// ���� GET ����Ĵ��루��֮ǰ��ͬ��
	}
	else if (method == "POST") {
		// ��������ͷ��������ķָ���
		size_t headerEnd = request.find("\r\n\r\n");

		if (headerEnd != string::npos) {
			// ��ȡ������
			string requestBody = request.substr(headerEnd + 4);

			// �����ﴦ�������壨���磬���������̨��
			cout << "�յ� POST ������: " << requestBody << endl;

			// ������Ӧ
			string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nReceived POST data";
			send(clientSocket, response.c_str(), response.length(), 0);
		}
		else {
			// �����ʽ���󣬷��� 400 Bad Request
			string badRequestMessage = "HTTP/1.1 400 Bad Request\r\n\r\n";
			send(clientSocket, badRequestMessage.c_str(), badRequestMessage.length(), 0);
		}
	}
	else {
		// ���� HTTP ���������� 400 Bad Request
		string badRequestMessage = "HTTP/1.1 400 Bad Request\r\n\r\n";
		send(clientSocket, badRequestMessage.c_str(), badRequestMessage.length(), 0);
	}
}

int main()
{
	WORD wsa = MAKEWORD(2, 2);
	WSADATA wsaData;

	if (WSAStartup(wsa, &wsaData) != 0)
	{
		cout << "��ʼ��ʧ��" << endl;
		return 0;
	}
	SOCKET s_server = socket(AF_INET, SOCK_STREAM, 0);
	
	if (s_server == INVALID_SOCKET) //-1�����־
	{
		cout << "socket����ʧ��" << endl;
		return 0;
	}

	//sever������Ϣ
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8888);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY; //�����������κνӿڵ�����

	//�����ʧ�ܵ����
	if (bind(s_server, (SOCKADDR *)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		cout << "ERROR bind!!" << endl;
		WSACleanup();	
	}

	if (listen(s_server, 5) < 0)
	{
		cout << "����ʧ��" << endl;
	}
	cout << "listen***************" << endl;
	sockaddr_in client_addr;
	SOCKET s_accept;
	int len = sizeof(SOCKADDR);
	s_accept = accept(s_server, (SOCKADDR*)&client_addr, &len);
	
	//��ʼ����,����ȴ�
	if (s_accept == SOCKET_ERROR)
	{
		cout << "connect error" << endl;
		return 0;
	}

	cout << "connection is OK, ready to accept" << endl;
	char recv_buf[100];
	char send_buf[100];
	//���ڻ�������ַ��ͷ����ַ�
	
	while (1) {
		sockaddr_in clientAddr;
		int addrLen = sizeof(clientAddr);
		SOCKET clientSocket = accept(s_server, (SOCKADDR*)&clientAddr, &addrLen);
		if (clientSocket == SOCKET_ERROR) {
			cout << "���Ӵ���" << endl;
			continue;
		}

		handleHttpRequest(clientSocket);
		closesocket(clientSocket);
	}
	
	closesocket(s_server);
	closesocket(s_accept);
	//�ͷ�DLL
	WSACleanup();
	return 0;	
}