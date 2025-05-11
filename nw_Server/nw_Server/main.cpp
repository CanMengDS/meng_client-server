#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "mswsock.lib") 
#include<iostream>
#include <cstdlib>
#include <cstring>
#include <thread>
#include<vector>
#include"MengSelectServer.h"
#include"MengTcpServer.h"
using namespace std;

#pragma pack(push, 1)

#define DEFAULT_PORT 5408
#define DEFAULT_ACCEPT_SOCKET_NUM 64
#define DEFAULT_WOKER 4
#define DEFAULT_BUFFER_SIZE 1024

void MPostAccept(SOCKET listen_socket)
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

		//if (WSAGetLastError() == WSA_IO_PENDING) break;
	}
	clog << "achieve post Accept completion" << '\n';
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
			
			cout << "awa" << endl;
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

			DWORD data_num = 0, flag = 0;

			string infor_ = "hello client! this is server,it's default message";
			size_t real_default = DEFAULT_BUFFER_SIZE - sizeof(MengDataHeader);

			MengDataHeader header;
			header.chunk_total = infor_.size() / (DEFAULT_BUFFER_SIZE - sizeof(MengDataHeader));
			header.total_size = infor_.size();
			header.present_chunk = 0;
			header.type = DATA_TYPE::STRING;

			if (infor_.size() % (DEFAULT_BUFFER_SIZE - sizeof(MengDataHeader)) != 0) {
				header.chunk_total + 1;
			}
			if (infor_.size() > (DEFAULT_BUFFER_SIZE - sizeof(MengDataHeader))) {

				size_t offest = 0; //记录读取到哪个位置
				const char* data_top = infor_.data(); //获取字符串以C字符串风格形式的头地址
				const size_t data_size = infor_.size(); //获取字符串所占总字节数
				size_t chunk_index = 0; //初始化当前处于碎片索引为0(下方header.present_part时需要用到)
				vector<vector<char>> temp_vec; //嵌套的vector更利于管理数据
				while (offest < data_size) { //当偏移量等于data_size时说明已经将字符串分割完并塞入嵌套的vector中

					header.present_chunk = chunk_index;					
					/*
					* 判断当前实际存入vector的字节数,如果到头了那么数据可能小于1024，就返回小的(实际剩下的)，如果没有到头那么1024就会最小，因此返回1024
					*/
					size_t real_remain = infor_.size() - offest;
					size_t data_present = real_default > real_remain ? real_remain : real_default;
					//vector<char> chunk(DEFAULT_BUFFER_SIZE); //临时存放碎片数据的地方
					memcpy(overlp->wasBuf.buf, &header, sizeof(MengDataHeader));
					memcpy(overlp->wasBuf.buf + sizeof(MengDataHeader), data_top + offest, data_present);
					
					//memcpy(chunk.data(), &header, sizeof(MengDataHeader));  //添加数据头
					//memcpy(chunk.data() + sizeof(MengDataHeader), data_top + offest, data_present); //因为要从chunk中数据头所占字节后开始拷贝，所以要加在其之后。
					//temp_vec.push_back(chunk);
					offest += data_present;
					chunk_index++;  //当前碎片索引
				}
			}else{
				header.persent_size = header.total_size;
				ZeroMemory(&overlp->overlapped, sizeof(OVERLAPPED));
				memset(overlp->buffer, 0, sizeof(overlp->buffer));
				overlp->type = IO_TYPE::IO_SEND;
				memcpy(overlp->wasBuf.buf, &header, sizeof(MengDataHeader));
				memcpy(overlp->wasBuf.buf + sizeof(MengDataHeader),infor_.data(),infor_.size());
				//overlp->wasBuf.buf = infor_.data();
				//overlp->wasBuf.len = infor_.size();
				WSASend(overlp->socket, &overlp->wasBuf, 1, &data_num, 0, &(overlp->overlapped), 0);
				cout << "已成功按预定发送指定数据..." << endl;
			}

			
		}
		break;
		case IO_TYPE::IO_SEND:
		{
			cout << "发送数据:" << bytesTrans << endl;
			ZeroMemory(&overlp->overlapped, sizeof(OVERLAPPED));
			memset(overlp->buffer, 0, sizeof(overlp->buffer));
			overlp->type = IO_TYPE::IO_RECV;
			overlp->wasBuf.len = DEFAULT_BUFFER_SIZE;
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
		MPostAccept(pms.listen_socket);
		
	}

	thread t1([] {
		while (1) {}
		});
	t1.join();
}