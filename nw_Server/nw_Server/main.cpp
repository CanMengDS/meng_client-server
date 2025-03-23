#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "mswsock.lib") 
#include<iostream>
#include <cstdlib>
#include <cstring>
#include <thread>
#include"MengMultiTcpConduct.h"
#include"MengTcpServer.h"
using namespace std;

#define DEFAULT_PORT 5408
#define DEFAULT_ACCEPT_SOCKET_NUM 64
#define DEFAULT_WOKER 4


bool MPostAccept(SOCKET overlapped_listen_socket)
{
	SOCKET client_sock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (client_sock == INVALID_SOCKET) return false;

	OverlappedPerIO* overlp = new OverlappedPerIO;
	if (overlp == nullptr) {
		closesocket(client_sock);
		return false;
	}

	ZeroMemory(overlp, sizeof(OverlappedPerIO));
	overlp->socket = client_sock;
	overlp->wasBuf.buf = overlp->buffer;
	overlp->wasBuf.len = 1024;
	overlp->type = IO_TYPE::IO_ACCEPT;
	DWORD recv_buf_num = 0;

	bool Succ = AcceptEx(overlapped_listen_socket, client_sock, overlp->wasBuf.buf, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &recv_buf_num, (LPOVERLAPPED)overlp);
	if (!Succ) {
		if (WSAGetLastError() == WSA_IO_PENDING) {
			cerr << "PostAccept is fail..." << endl;
		}
		return false;
	}
	clog << "有新的客户端连接..." << endl;
	return true;
}

DWORD WINAPI MwokerThread(LPVOID lp) {
	cout << "wokerThread is woker" << '\n';
	ServerParams* pms = new ServerParams;
	pms = (ServerParams*)lp;

	HANDLE completionPort = pms->iocp;
	SOCKET listenSocket = pms->listen_socket;

	DWORD bytesTrans;
	ULONG_PTR Key;
	OverlappedPerIO* overlp;
	while (1) {
		bool result = GetQueuedCompletionStatus(completionPort, &bytesTrans, &Key, (LPOVERLAPPED*)&overlp, INFINITE);
		if (!result) {
			if ((GetLastError() == WAIT_TIMEOUT) || (GetLastError() == ERROR_NETNAME_DELETED)) {
				cout << "连接异常,超时绕过|监听客户端为:" << overlp->socket << endl;
				closesocket(overlp->socket);
				delete overlp;
				continue;
			}
			cout << "获取完成端口结果失败" << endl;
			return 0;
		}

		DWORD data_num = 0, flag = 0;
		switch (overlp->type) {
		case IO_TYPE::IO_ACCEPT:
		{
			MPostAccept(pms->listen_socket);
			setsockopt(overlp->socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&(listenSocket), sizeof(SOCKET));

			ZeroMemory(overlp->buffer, 1024);
			overlp->type = IO_TYPE::IO_RECV;
			overlp->wasBuf.buf = overlp->buffer;
			overlp->wasBuf.len = 1024;
			CreateIoCompletionPort((HANDLE)overlp->socket, completionPort, NULL, 0);


			WSARecv(overlp->socket, &overlp->wasBuf, 1, &data_num, &flag, &(overlp->overlapped), 0);
		}
		break;
		case IO_TYPE::IO_RECV:
		{

			if (bytesTrans == 0) {
				cout << "客户端断开:" << overlp->socket << endl;
				closesocket(overlp->socket);
				delete overlp;
				continue;
			}
			cout << "[" << overlp->socket << "]" << ":" << overlp->buffer << endl;

			ZeroMemory(&overlp->overlapped, sizeof(OVERLAPPED));
			overlp->wasBuf.buf = overlp->buffer;
			overlp->wasBuf.len = 1024;
			DWORD data_num = 0, flag = 0;
			WSARecv(overlp->socket, &overlp->wasBuf, 1, &data_num, &flag, &(overlp->overlapped), 0);
		}
		break;
		}
	}
	return 0;
}

int main() {
	MengTcpServer mengServer;
	ServerParams pms{ 0 };
	if (mengServer.initServer(DEFAULT_PORT,pms) == false) {
		cerr << "连接未成功..." << endl;
	}
	clog << "网络及通信初始化完毕 | the server network and listen socket initialization completed..." << '\n' << endl;
	int choies = 0;
	cout << "请输入\"1\"确认开启通信并接收数据" << endl;
	cin >> choies;
	
	switch (choies) {
	case 1:
		auto func = std::bind(MwokerThread, &pms);
		for (int i = 0; i < DEFAULT_WOKER; i++) {
			CreateThread(NULL, 0, MwokerThread, &pms, 0, NULL);
		}

		for (int i = 0; i <= DEFAULT_ACCEPT_SOCKET_NUM; i++) {
			mengServer.PostAccept(pms.listen_socket);
		}
		clog << "异步投递完毕 | accEx post completed..." << endl;
	}

	thread t1([] {
		while (1) {}
		});
	t1.join();
}