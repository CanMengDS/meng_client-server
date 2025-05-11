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
	fd_set read_set; //原始set,保存需要检测的句柄
	fd_set temp_read_set; //原始set的备份，用来调用select时传入
};