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

	// 将接收的数据转换为字符串
	string request(recvBuffer, recvLength);

	// 解析 HTTP 请求行
	stringstream ss(request);
	string method, uri, version;
	ss >> method >> uri >> version;

	if (method == "GET") {
		// 处理 GET 请求的代码（与之前相同）
	}
	else if (method == "POST") {
		// 查找请求头和请求体的分隔符
		size_t headerEnd = request.find("\r\n\r\n");

		if (headerEnd != string::npos) {
			// 提取请求体
			string requestBody = request.substr(headerEnd + 4);

			// 在这里处理请求体（例如，输出到控制台）
			cout << "收到 POST 请求体: " << requestBody << endl;

			// 发送响应
			string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nReceived POST data";
			send(clientSocket, response.c_str(), response.length(), 0);
		}
		else {
			// 请求格式错误，发送 400 Bad Request
			string badRequestMessage = "HTTP/1.1 400 Bad Request\r\n\r\n";
			send(clientSocket, badRequestMessage.c_str(), badRequestMessage.length(), 0);
		}
	}
	else {
		// 其他 HTTP 方法，发送 400 Bad Request
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
		cout << "初始化失败" << endl;
		return 0;
	}
	SOCKET s_server = socket(AF_INET, SOCK_STREAM, 0);
	
	if (s_server == INVALID_SOCKET) //-1错误标志
	{
		cout << "socket创建失败" << endl;
		return 0;
	}

	//sever配置信息
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8888);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY; //服务器监听任何接口的连接

	//处理绑定失败的情况
	if (bind(s_server, (SOCKADDR *)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		cout << "ERROR bind!!" << endl;
		WSACleanup();	
	}

	if (listen(s_server, 5) < 0)
	{
		cout << "监听失败" << endl;
	}
	cout << "listen***************" << endl;
	sockaddr_in client_addr;
	SOCKET s_accept;
	int len = sizeof(SOCKADDR);
	s_accept = accept(s_server, (SOCKADDR*)&client_addr, &len);
	
	//开始阻塞,挂起等待
	if (s_accept == SOCKET_ERROR)
	{
		cout << "connect error" << endl;
		return 0;
	}

	cout << "connection is OK, ready to accept" << endl;
	char recv_buf[100];
	char send_buf[100];
	//用于缓存接受字符和发送字符
	
	while (1) {
		sockaddr_in clientAddr;
		int addrLen = sizeof(clientAddr);
		SOCKET clientSocket = accept(s_server, (SOCKADDR*)&clientAddr, &addrLen);
		if (clientSocket == SOCKET_ERROR) {
			cout << "连接错误" << endl;
			continue;
		}

		handleHttpRequest(clientSocket);
		closesocket(clientSocket);
	}
	
	closesocket(s_server);
	closesocket(s_accept);
	//释放DLL
	WSACleanup();
	return 0;	
}