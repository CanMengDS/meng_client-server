#pragma once
#include<iostream>
#include<string>
#include<WinSock2.h>
#include<WS2tcpip.h>
using namespace std;

class MengTcpClient {
public:
	bool tcp_connect(const char* in_server_ip, const unsigned short in_port); //��Ҫ����˵�ip��ͨѶ�˿�,�����˷������ӣ��ɹ�true��ʧ��false
	bool send(const char*, size_t len, SOCKET tcp_socket);
	static bool recv(char* buffer, const size_t buffer_len, SOCKET tcp_socket);
	SOCKET getClientSocket();
	MengTcpClient();
	~MengTcpClient();
private:
	SOCKET client_socket;  // �ͻ��˵�socket��-1��ʾ�Ͽ���>=0��ʾ��Ч
	string server_ip;  //������ip
	string cl_port; //ͨѶ�˿�

};