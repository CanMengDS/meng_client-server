#include<WinSock2.h>
#include <ws2tcpip.h>
#include<MSWSock.h>
#include <cstdint>




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

struct MengDataHeader {
	DATA_TYPE type;
	uint32_t total_size;
	uint16_t persent_size;
	uint8_t present_chunk;
	uint8_t chunk_total;
};

struct OverlappedPerIO {
	OVERLAPPED overlapped;
	SOCKET socket;
	WSABUF wasBuf;
	IO_TYPE type;
	char buffer[1024];
};

typedef struct {
	SOCKET client_socket;
} PER_HANDLE_DATA;

struct ServerParams {
	SOCKET listen_socket;
	HANDLE iocp;
};