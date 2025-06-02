#include"../include/CMIOCPTcpServer.h"

CMIOCPTcpServer::CMIOCPTcpServer()
{
	WSAData wsa_data;
	if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
		cout << "未知原因,网络初始化失败,已断开程序网络..." << endl;
		return;
	}
	cout << "正确初始化网络..." << endl;
}

CMIOCPTcpServer::~CMIOCPTcpServer()
{

}

void CMIOCPTcpServer::PostAccept(SOCKET listen_socket)
{
	SOCKET client_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (client_socket == INVALID_SOCKET) return;
	OverlappedExtendIo* overlp = new OverlappedExtendIo;
	if (overlp == nullptr) {
		closesocket(overlp->socket);
		return;
	}
	ZeroMemory(overlp, sizeof(OverlappedExtendIo));
	overlp->wsaBuf.buf = overlp->buffer;
	overlp->wsaBuf.len = DEFAULT_BUFFER_SIZE;
	overlp->socket = client_socket;
	overlp->type = IO_TYPE::IO_ACCEPT;
	overlp->uid = htonl(2025);
	
	DWORD recv_num = 0;
	bool result = AcceptEx(listen_socket,
		client_socket,
		&overlp->wsaBuf,
		0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		&recv_num, (LPOVERLAPPED)overlp);
	cout << "成功投递,有新客户端接入" << endl;
}

bool CMIOCPTcpServer::InitServer(unsigned tcp_port,ServerParams& parms){
	parms.sock = WSASocket(AF_INET, SOCK_STREAM,0,NULL,0,WSA_FLAG_OVERLAPPED);
	if (parms.sock == INVALID_SOCKET) {
		cerr << "创建网络套接字失败..." << endl;
		return false;
	}
	//this->SetListenSocket(tcp_port);
	//parms.sock = parms.server->GetListenSocket();

	sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(tcp_port);
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (::bind(parms.sock, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
		closesocket(parms.sock);
		cerr << "服务端绑定失败" << endl;
		return false;
	}

	if (::listen(parms.sock, 5) == SOCKET_ERROR) {
		closesocket(parms.sock);
		parms.sock = INVALID_SOCKET;
		cerr << "服务器端监听失败" << endl;
		return false;
	}

	if ((parms.iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0)) != NULL) {
		if (parms.iocp == NULL) {
			cerr << "创建完成端口失败" << endl;
		}

		if (CreateIoCompletionPort((HANDLE)parms.sock, parms.iocp, NULL, 0) != NULL) {
			return true;
		}
		CloseHandle(parms.iocp);
	}
	cerr << "初始化服务器发生错误" << endl;
	closesocket(parms.sock);
	return false;
}

void CMIOCPTcpServer::IOCPWokerThread(ServerParams& pms)
{
	clog << "woker_thread is init achieve" << endl;

	HANDLE iocp = pms.iocp;
	SOCKET listen_socket = pms.sock;

	DWORD byte_num;
	ULONG_PTR key;
	OverlappedExtendIo* overlp;

	string infor_ = "hello client! this is server,it's default message";
	size_t default_size = DEFAULT_BUFFER_SIZE - sizeof(CMDataHeader);

	while (1) {
		bool get_result = GetQueuedCompletionStatus(iocp, &byte_num, &key, (LPOVERLAPPED*)&overlp, INFINITE);
		if (!get_result) {
			if ((GetLastError() == WAIT_TIMEOUT) || (GetLastError() == ERROR_NETNAME_DELETED)) {
				cout << "有客户端断开连接:" << overlp->socket << endl;
				CancelIoEx((HANDLE)overlp->socket, &overlp->overlapped);
				closesocket(overlp->socket);
				delete overlp;
				continue;
			}
		}

		DWORD data_num = 0, flag = 0;
		switch (overlp->type) {
		case IO_TYPE::IO_ACCEPT:
		{
			this->PostAccept(listen_socket);

			setsockopt(overlp->socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&(listen_socket), sizeof(SOCKET));
			ZeroMemory(overlp->buffer, sizeof(DEFAULT_BUFFER_SIZE));
			overlp->type = IO_TYPE::IO_RECV;
			overlp->wsaBuf.buf = overlp->buffer;
			overlp->wsaBuf.len = DEFAULT_BUFFER_SIZE;
			CreateIoCompletionPort((HANDLE)overlp->socket, iocp, NULL, 0);
			WSARecv(overlp->socket, &overlp->wsaBuf, 1, &data_num, &flag, &(overlp->overlapped), 0);
		}
		break;
		case IO_TYPE::IO_RECV: 
		{
			if (byte_num == 0) {
				cout << "客户端断开连接..." << overlp->socket << endl;
				closesocket(overlp->socket);
				delete overlp;
				continue;
			}
			cout << "[" << overlp->socket << "]:" << overlp->buffer << endl;

			CMDataHeader header;
			header.total_size = htonl(infor_.size());
			header.chunk_total = infor_.size() / default_size;
			header.present_chunk = 1;
			header.persent_size = infor_.size();
			header.data_signs = htons(5408);
			header.type = DATA_TYPE::STRING;
			
			ZeroMemory(&overlp->overlapped, sizeof(OVERLAPPED));
			memset(overlp->buffer, 0, sizeof(overlp->wsaBuf.len));
			overlp->type = IO_TYPE::IO_SEND;
			overlp->wsaBuf.buf = overlp->buffer;
			overlp->wsaBuf.len = DEFAULT_BUFFER_SIZE;
			memcpy(overlp->wsaBuf.buf, &header, sizeof(CMDataHeader));
			memcpy(overlp->wsaBuf.buf + sizeof(CMDataHeader), infor_.data(), infor_.size());
			WSASend(overlp->socket, &overlp->wsaBuf, 1, &byte_num, 0, &(overlp->overlapped), 0);
			cout << "已按预定成功发送数据:" << overlp->buffer << endl;
		}
		break;
		case IO_TYPE::IO_SEND:
		{
			cout << "实际发送数据量:" << byte_num << endl;

			ZeroMemory(&overlp->overlapped, sizeof(OVERLAPPED));
			memset(overlp->buffer, 0, sizeof(overlp->wsaBuf));
			overlp->type = IO_TYPE::IO_RECV;
			overlp->wsaBuf.len = DEFAULT_BUFFER_SIZE;
			WSARecv(overlp->socket, &overlp->wsaBuf, 1, &data_num, &flag, &(overlp->overlapped), 0);
		}
		break;
		}
	}
}
