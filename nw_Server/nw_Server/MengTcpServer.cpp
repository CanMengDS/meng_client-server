#include "MengTcpServer.h"
#define BUFFER_SIZE 1024

int MengTcpServer::getRandomFileSign(short max_random_base)
{
	for (int i = 1; i <= max_random_base; i++) {
		random_base.push_back(i);
	}
	sample(random_base.begin(), random_base.end(), random_out.begin(), 3, mt19937{ random_device{}() });
	return *random_out.begin() * 100 + *(random_out.begin() + 1) * 10 + *(random_out.begin() + 2);
}

bool MengTcpServer::initServer(const unsigned short in_port,ServerParams& pms)
{
	pms.listen_socket = WSASocket(AF_INET, SOCK_STREAM, 0,NULL,0,WSA_FLAG_OVERLAPPED);
	if (pms.listen_socket == INVALID_SOCKET) return false;

	sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(in_port);
	servaddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (::bind(pms.listen_socket, (sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
		closesocket(pms.listen_socket);
		return false;
	}
	if (listen(pms.listen_socket, 5) == -1) {
		closesocket(pms.listen_socket);
		pms.listen_socket = -1;
		return false;
	}

	if ((pms.iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0)) != NULL) {
		if (pms.iocp == NULL) {
			cout << "创建完成端口失败" << endl;
			return false;
		}
		if (CreateIoCompletionPort((HANDLE)pms.listen_socket, pms.iocp, NULL, 0) != NULL) {
			return true;
		}
		CloseHandle(pms.iocp);
	}
	cerr << "关联完成端口失败" << endl;
	closesocket(pms.listen_socket);
	return false;
}


void MengTcpServer::PostAccept(SOCKET listen_socket)
{
	SOCKET client_sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (client_sock == INVALID_SOCKET) return;

	OverlappedPerIO* overlp = new OverlappedPerIO;
	if (overlp == nullptr) {
		closesocket(client_sock);
		return;
	}

	ZeroMemory(overlp, sizeof(OverlappedPerIO));
	overlp->socket = client_sock;
	overlp->wasBuf.buf = overlp->buffer;
	overlp->wasBuf.len = 1024;
	overlp->type = IO_TYPE::IO_ACCEPT;
	DWORD recv_buf_num = 0;


	while (AcceptEx(listen_socket,
		client_sock,
		overlp->wasBuf.buf,
		0,
		sizeof(SOCKADDR_IN) + 16,
		sizeof(SOCKADDR_IN) + 16,
		&recv_buf_num, (LPOVERLAPPED)overlp)) {
	}
	clog << "achieve post Accept completion" << '\n';
}

void MengTcpServer::wokerThread(ServerParams lp) {
	cout << "wokerThread is woker" << endl;
	ServerParams pms = lp;
	HANDLE completionPort = pms.iocp;
	SOCKET listenSocket = pms.listen_socket;

	DWORD bytesTrans;
	ULONG_PTR Key;
	OverlappedPerIO* overlp;
	while (1) {
		bool result = GetQueuedCompletionStatus(completionPort, &bytesTrans, &Key, (LPOVERLAPPED*)&overlp, INFINITE);
		if (!result) {
			//错误处理逻辑，为了省事赶时间，不写了,后面看情况补回来了。。
			closesocket(overlp->socket);
			//delete overlp;
			return;
		}

		DWORD data_num = 0, flag = 0;
		switch (overlp->type) {
		case IO_TYPE::IO_ACCEPT:
			PostAccept(pms.listen_socket);
			setsockopt(overlp->socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&(listenSocket), sizeof(SOCKET));

			ZeroMemory(overlp->buffer, 1024);
			overlp->type = IO_TYPE::IO_RECV;
			overlp->wasBuf.buf = overlp->buffer;
			overlp->wasBuf.len = 1024;
			CreateIoCompletionPort((HANDLE)overlp->socket, completionPort, NULL, 0);

			
			WSARecv(overlp->socket, &overlp->wasBuf, 1, &data_num, &flag, &(overlp->overlapped), 0);
			break;
		case IO_TYPE::IO_RECV:

			clog << "有新的数据到达..." << endl;
			if (bytesTrans == 0) {
				closesocket(overlp->socket);
				continue;
			}
			cout << "接收数据:" << overlp->buffer << endl;
			break;


			ZeroMemory(&overlp->overlapped, sizeof(OVERLAPPED));
			overlp->type = IO_TYPE::IO_SEND;
			char ms[1024] = "null";
			overlp->wasBuf.buf = ms;
			overlp->wasBuf.len = strlen(ms);

			DWORD dsn = strlen(ms);
			WSASend(overlp->socket, &overlp->wasBuf, 1, &dsn, 0, &(overlp->overlapped), 0);
		}
		
	}
	
}


/**
 * @brief 
 * @param buffer 同下
 * @param maxlen 缓冲区大小
 * @param tcp_socket
 * @return 
 */
bool MengTcpServer::recv(char* buffer, const size_t buffer_len,SOCKET tcp_socket)
{
	memset(buffer, 0, buffer_len);
	int readn = ::recv(tcp_socket, buffer, buffer_len - 1, 0);
	if (readn <= 0) {
		memset(buffer, 0, buffer_len);
		return false;
	}
	return true;
}


/**
 * @brief 用于阻塞通信以及select的send函数
 * @param buffer 应填入一个char数组
 * @param buffer_len 数组长度
 * @param tcp_socket 
 * @return 
 */
bool MengTcpServer::send(const char buffer[1024], const size_t buffer_len, SOCKET tcp_socket)
{
	if (tcp_socket == INVALID_SOCKET)return false;
	if (::send(tcp_socket, buffer, sizeof(buffer), 0) <= 0) return false;
	return true;
}

//bool MengTcpServer::closeListenSocket()
//{
//	if (server_listen_socket == -1) return false;
//	closesocket(server_listen_socket);
//	return true;
//}

bool MengTcpServer::closeServerTcpSocket()
{
	if (tcp_socket == -1)return false;
	closesocket(tcp_socket);
	tcp_socket = -1;
	return true;
}

const string& MengTcpServer::getClientIp() const
{
	return client_ip;
}

int MengTcpServer::getServerListenSocket()
{
	return this->server_listen_socket;
}

MengTcpServer::MengTcpServer() :server_listen_socket(-1) //初始化客户端,服务端初始ip(客户端还未连接)
{
	//threads_pool = new MengThreadPool(8);
	WSADATA wsaData;     
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) 
	{ 
		throw runtime_error("网络初始化失败..."); 
	}
}

MengTcpServer::~MengTcpServer()
{
	//closeListenSocket();
	//closeServerTcpSocket();
}
