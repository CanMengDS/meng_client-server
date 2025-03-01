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
	fd_set read_set; //ԭʼset,������Ҫ���ľ��
	fd_set temp_read_set; //ԭʼset�ı��ݣ���������selectʱ����
};