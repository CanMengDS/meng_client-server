#pragma once
#include<functional>
#include<vector>
#include"MengTcpServer.h"

class MengMultiTcpConduct {
public:
	MengMultiTcpConduct();
	SOCKET initExchange(SOCKET server_listen_socket, function<bool(string& buffer, const size_t maxlen, int tcp_socket)> recv_document_function);
private:
	vector<string> client_ips;
	string buffer;
	fd_set read_set; //原始set,保存需要检测的句柄
	fd_set temp_read_set; //原始set的备份，用来调用select时传入
};