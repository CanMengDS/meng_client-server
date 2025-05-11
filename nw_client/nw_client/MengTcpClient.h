#pragma once
#include<iostream>
#include<string>
#include<WinSock2.h>
#include<WS2tcpip.h>
using namespace std;

class MengTcpClient {
public:
	bool tcp_connect(const char* in_server_ip, const unsigned short in_port); //需要服务端的ip和通讯端口,向服务端发起连接，成功true，失败false
	bool send(const char*, size_t len, SOCKET tcp_socket);
	static bool recv(char* buffer, const size_t buffer_len, SOCKET tcp_socket);
	SOCKET getClientSocket();
	MengTcpClient();
	~MengTcpClient();
private:
	SOCKET client_socket;  // 客户端的socket，-1表示断开，>=0表示有效
	string server_ip;  //服务器ip
	string cl_port; //通讯端口

};