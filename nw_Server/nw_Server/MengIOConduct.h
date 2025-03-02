#pragma once
#include<iostream>
#include<fstream>
#include<filesystem>
#include"MengTcpServer.h"
using namespace std;

class MengIOConduct {
public:
	MengIOConduct() = default;
	bool recvive(SOCKET tcp_socket);
	bool sendBinaryFile(SOCKET tcp_socket, string path);
private:
	char buffer[1024];
};