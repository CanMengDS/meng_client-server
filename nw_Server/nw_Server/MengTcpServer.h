#pragma once
#include<iostream>
#include<WinSock2.h>
#include <ws2tcpip.h>
using namespace std;

class MengTcpServer {
private:
	int server_listen_socket; //监听socket
	//int server_tcp_socket; //通讯socket
	char client_ip[INET_ADDRSTRLEN]; //客户端ip
	unsigned short se_port; //服务端通讯端口
public:
	bool initServer(const unsigned short in_port); //初始化服务端监听socket/服务端
	pair<int,bool> accept();
	//bool send(const string &buffer, int connect_socket);
	static bool recv(char* buffer, const size_t buffer_len, const SOCKET tcp_socket);
	//static bool recv(void* buffer, const size_t bf_size, const SOCKET tcp_socket);
	//static bool send(void* buffer, const size_t bf_size, const SOCKET tcp_socket);
	static bool send(const char buffer[1024], const size_t buffer_len, const SOCKET tcp_socket);
	bool closeListenSocket(); //关闭监听socket
	//bool closeServerTcpSocket(); //关闭通讯socket
	const string& getClientIp() const;
	int getServerListenSocket();
	MengTcpServer();
	~MengTcpServer();
};