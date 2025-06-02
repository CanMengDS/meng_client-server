#pragma once
#include <iostream>
#include"../include/CMIOCPTcpServer.h"
using namespace std;

class CMIOCPTcpServer;


enum class IO_TYPE {
	IO_ACCEPT,
	IO_SEND,
	IO_RECV,
	IO_CONNECT,
	IO_DISCONNECT
};

enum class DATA_TYPE {
	STRING,
	TXT,
	JPG
};

struct CMDataHeader {
	DATA_TYPE type;
	uint32_t total_size;
	uint16_t persent_size;
	uint8_t present_chunk;
	uint8_t chunk_total;
	uint16_t data_signs;
};

struct OverlappedExtendIo {
	OVERLAPPED overlapped;
	SOCKET socket;
	WSABUF wsaBuf;
	IO_TYPE type;
	char buffer[1024];
	uint32_t uid;
};

struct ServerParams {
	HANDLE iocp;
	SOCKET sock;
	CMIOCPTcpServer* server;
};