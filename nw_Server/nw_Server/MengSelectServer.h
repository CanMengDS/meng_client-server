#pragma once
#include<functional>
#include<vector>
#include"MengTcpServer.h"

class MengSelectServer {
public:
	MengSelectServer();
	SOCKET initExchange(SOCKET server_listen_socket);
private:
	vector<string> client_ips;
	char buffer[1024];
	char buffer_t[1024];
	fd_set read_set; //ԭʼset,������Ҫ���ľ��
	fd_set temp_read_set; //ԭʼset�ı��ݣ���������selectʱ����
};