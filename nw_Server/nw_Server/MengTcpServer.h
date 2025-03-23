#pragma once
#include<iostream>
#include"IOconc.h"
using namespace std;

class MengTcpServer {
private:
	SOCKET server_listen_socket = 0;
	
	int tcp_socket;
	HANDLE iocp;
	char client_ip[INET_ADDRSTRLEN]; //客户端ip
	unsigned short se_port; //服务端通讯端口
public:

	bool initServer(const unsigned short in_port, ServerParams& pms); //初始化服务端监听socket/服务端
	//static pair<int,bool> PostAccept(SOCKET listen_socket);
	void wokerThread(ServerParams lp);
	void PostAccept(SOCKET overlapped_listen_socket);
	static bool recv(char* buffer, const size_t buffer_len, SOCKET tcp_socket);
	static bool send(const char buffer[1024], const size_t buffer_len, SOCKET tcp_socket);
	//bool closeListenSocket(); //关闭监听socket
	bool closeServerTcpSocket(); //关闭通讯socket
	const string& getClientIp() const;
	int getServerListenSocket();
	MengTcpServer();
	~MengTcpServer();
};