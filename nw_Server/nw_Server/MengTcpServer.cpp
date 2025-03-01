#include "MengTcpServer.h"

bool MengTcpServer::initServer(const unsigned short in_port)
{
	if ((server_listen_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)return false;
	se_port = in_port;

	sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(5408);
	servaddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (::bind(server_listen_socket, (sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
		closesocket(server_listen_socket);
		return false;
	}
	if (listen(server_listen_socket, 5) == -1) {
		closesocket(server_listen_socket);
		server_listen_socket = -1;
		return false;
	}
	return true;
}

pair<int,bool> MengTcpServer::accept()
{
	sockaddr_in cddr; //客户端的地址信息
	int addrlen = sizeof(cddr);  
	int temp_socket = 0;
	if ((temp_socket = ::accept(server_listen_socket, (sockaddr*)&cddr, &addrlen)) == INVALID_SOCKET) return pair<int, bool>(0, false);
	const char* client_ips = inet_ntop(AF_INET,&(cddr.sin_addr.S_un.S_addr), client_ip, sizeof(client_ip));
	clog << "成功与客户端建立连接" << "(" << client_ips << ":" << cddr.sin_port<<")" << endl;
	//createClientExchange(temp_socket,cddr,client_ips);
	return pair<int,bool>(temp_socket,true);
}

/**
 * @brief 
 * @param buffer 同上
 * @param maxlen 缓冲区大小，因为使用clear清除了buffer的内容，因此指定一个最大缓冲大小并在调用recv接收数据时有用到。
 * @param tcp_socket 同上
 * @return 
 */
bool MengTcpServer::recv(string& buffer, const size_t maxlen,int tcp_socket)
{
	buffer.clear();
	buffer.resize(maxlen);
	int readn = ::recv(tcp_socket, &buffer[0], buffer.size(), 0);
	if (readn <= 0) {
		buffer.clear();
		return false;
	}
	buffer.resize(readn);
	return true;
}

bool MengTcpServer::closeListenSocket()
{
	if (server_listen_socket == -1) return false;
	closesocket(server_listen_socket);
	return true;
}

//bool MengTcpServer::closeServerTcpSocket()
//{
////	if (server_tcp_socket == -1)return false;
//	closesocket(server_tcp_socket);
//	server_tcp_socket = -1;
//	return true;
//}

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
	closeListenSocket();
	//closeServerTcpSocket();
}
