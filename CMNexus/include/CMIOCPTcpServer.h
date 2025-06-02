#pragma once
#include"../include/CMAbstractTcpServer.h"
#include"../include/CMNexus.h"
#include"../include/NetCoreDefs.h"




class CMIOCPTcpServer : protected CMAbstractTcpServer {
public:
	CMIOCPTcpServer();
	~CMIOCPTcpServer();
	void PostAccept(SOCKET listen_socket);
	bool InitServer(unsigned tcp_port, ServerParams& parms);
	void IOCPWokerThread(ServerParams& pms);
};