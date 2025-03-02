#pragma once
#include<iostream>
#include<fstream>
#include<filesystem>
#include"MengTcpClient.h"
using namespace std;

class MengIOConduct {
public:
	MengIOConduct() = default;
	bool recvive(SOCKET tcp_socket, MengTcpClient& tcp_client);
	bool sendBinaryFile(SOCKET tcp_socket, string path, MengTcpClient& tcp_client);
private:
	char buffer[1024];
};