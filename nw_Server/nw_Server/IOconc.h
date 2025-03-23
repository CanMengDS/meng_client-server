#include<WinSock2.h>
#include <ws2tcpip.h>
#include<MSWSock.h>

enum class IO_TYPE {
	IO_ACCEPT,
	IO_SEND,
	IO_RECV,
	IO_CONNECT,
	IO_DISCONNECT
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