#pragma once
//#pragma pack(push, 1)
#include<WinSock2.h>
#include <ws2tcpip.h>
#include<MSWSock.h>
#include <cstdint>
#include"MengTcpServer.h"



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

#pragma pack(push, 1)
struct MengDataHeader {
	DATA_TYPE type;
	uint32_t total_size;
	uint16_t persent_size;
	uint8_t present_chunk;
	uint8_t chunk_total;
	uint16_t data_signs;
};
#pragma pack(pop) 
struct OverlappedPerIO {
	OVERLAPPED overlapped;
	SOCKET socket;
	WSABUF wasBuf;
	IO_TYPE type;
	char buffer[1024];
	uint16_t id;
};

typedef struct {
	SOCKET client_socket;
} PER_HANDLE_DATA;

struct ServerParams {
	SOCKET listen_socket;
	HANDLE iocp;
};

struct UnTreatedData {
	const char* top;
	size_t total;
	size_t offest;
	int total_chunk;
	int present_chunk;
	SOCKET socket;
};

struct TempParamServerInfor {
	ServerParams* pms;
	class MengTcpServer* serv;
};