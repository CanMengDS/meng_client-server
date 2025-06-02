#pragma once

#include<WinSock2.h>
#include<MSWSock.h>
#include<WS2tcpip.h>


class CMAbstractTcpServer {
public:
	CMAbstractTcpServer();
	~CMAbstractTcpServer();
	virtual bool CloseListenSocket();
	virtual bool CLoseTcpServerSocket();
protected:
	void SetListenSocket(SOCKET listen_socket);
	void SetTcpSocket(SOCKET tcp_socket);
	SOCKET GetListenSocket();
	SOCKET GetTcpSocket();
private:
	SOCKET listen_socket;
	SOCKET tcp_socket;
	unsigned tcp_port;
};
