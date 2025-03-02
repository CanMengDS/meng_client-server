#pragma once
#include<iostream>
#include<WinSock2.h>
#include <ws2tcpip.h>
using namespace std;

class MengTcpServer {
private:
	int server_listen_socket; //����socket
	//int server_tcp_socket; //ͨѶsocket
	char client_ip[INET_ADDRSTRLEN]; //�ͻ���ip
	unsigned short se_port; //�����ͨѶ�˿�
public:
	bool initServer(const unsigned short in_port); //��ʼ������˼���socket/�����
	pair<int,bool> accept();
	//bool send(const string &buffer, int connect_socket);
	static bool recv(char* buffer, const size_t buffer_len, const SOCKET tcp_socket);
	//static bool recv(void* buffer, const size_t bf_size, const SOCKET tcp_socket);
	//static bool send(void* buffer, const size_t bf_size, const SOCKET tcp_socket);
	static bool send(const char buffer[1024], const size_t buffer_len, const SOCKET tcp_socket);
	bool closeListenSocket(); //�رռ���socket
	//bool closeServerTcpSocket(); //�ر�ͨѶsocket
	const string& getClientIp() const;
	int getServerListenSocket();
	MengTcpServer();
	~MengTcpServer();
};