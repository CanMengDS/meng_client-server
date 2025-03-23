#pragma once
#include<iostream>
#include"IOconc.h"
using namespace std;

class MengTcpServer {
private:
	SOCKET server_listen_socket = 0;
	
	int tcp_socket;
	HANDLE iocp;
	char client_ip[INET_ADDRSTRLEN]; //�ͻ���ip
	unsigned short se_port; //�����ͨѶ�˿�
public:

	bool initServer(const unsigned short in_port, ServerParams& pms); //��ʼ������˼���socket/�����
	//static pair<int,bool> PostAccept(SOCKET listen_socket);
	void wokerThread(ServerParams lp);
	void PostAccept(SOCKET overlapped_listen_socket);
	static bool recv(char* buffer, const size_t buffer_len, SOCKET tcp_socket);
	static bool send(const char buffer[1024], const size_t buffer_len, SOCKET tcp_socket);
	//bool closeListenSocket(); //�رռ���socket
	bool closeServerTcpSocket(); //�ر�ͨѶsocket
	const string& getClientIp() const;
	int getServerListenSocket();
	MengTcpServer();
	~MengTcpServer();
};