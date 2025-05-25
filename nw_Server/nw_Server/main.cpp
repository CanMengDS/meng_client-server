#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "mswsock.lib") 
#include<iostream>
#include <cstdlib>
#include <cstring>
#include <thread>

#include"MengSelectServer.h"
using namespace std;

#pragma pack(push, 1)

#define DEFAULT_PORT 5408
#define DEFAULT_ACCEPT_SOCKET_NUM 64
#define DEFAULT_WOKER 4
#define DEFAULT_BUFFER_SIZE 1024

DWORD WINAPI MwokerThread(LPVOID lp) {
	cout << "wokerThread is woker" << '\n';
	TempParamServerInfor* temp = (TempParamServerInfor*)lp;
	MengTcpServer* mengServer = temp->serv;
	ServerParams* pms = temp->pms;

	HANDLE completionPort = pms->iocp;
	SOCKET listenSocket = pms->listen_socket;

	DWORD bytesTrans;
	ULONG_PTR Key;
	OverlappedPerIO* overlp;

	string infor_K = "hello client! this is server,it's default message";
	string infor_ = "Nowadays, there is a increasing number for people to join in the activities of volunteering. It’s well-known that volunteering job is making contribution to the society with nothing in return. It’s like the Leifeng spirit. However, knowing this, there are still many people would like to devote themseves in this trend. For instance, the 20xx Beijing Olympic Games has so many volunteers. They made lots of contribution to the success of opening the Olympics. Moreover, we can often see that there many students go to gerocomium to visit the old to bring warm to the lonely people. Even there are some people’s career is volunteer. They are ready to help others. Although the volunteers are much more common, we still need to thanks them to admire them devoting themseves to the society.After dinner, we enjoy the Spring Festival Gala with great interest, but the most interesting thing is magic. At the beginning of the stairs, there were fireworks, and my father and I came to the balcony to see the fireworks. In the sky, the fireworks were colorful and varied. It was really beautiful! Fireworks are very beautiful, I hope you can consciously stop fireworks, for the environment for our survival can be better! Imperceptibly, television began to greet the new year countdown, I fell asleep in a happy daze, wake up on the second day, he heard the sound of firecrackers, my mother said I grow old. The most happy thing is that there is a red bag under the pillow. Ha ha...";
	size_t real_default = DEFAULT_BUFFER_SIZE - sizeof(MengDataHeader);

	

	while (1) {
		bool result = GetQueuedCompletionStatus(completionPort, &bytesTrans, &Key, (LPOVERLAPPED*)&overlp, INFINITE);
		if (!result) {
			if ((GetLastError() == WAIT_TIMEOUT) || (GetLastError() == ERROR_NETNAME_DELETED)) {
				cout << "连接异常,超时绕过|监听客户端为:" << overlp->socket << endl;
				CancelIoEx((HANDLE)overlp->socket, &overlp->overlapped);
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
			mengServer->PostAccept(pms->listen_socket);
			
			cout << "awa" << endl;
			setsockopt(overlp->socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&(listenSocket), sizeof(SOCKET));
			ZeroMemory(overlp->buffer, DEFAULT_BUFFER_SIZE);
			overlp->type = IO_TYPE::IO_RECV;
			overlp->wasBuf.buf = overlp->buffer;
			overlp->wasBuf.len = DEFAULT_BUFFER_SIZE;
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

			MengDataHeader header;
			header.total_size = htonl(infor_.size());
			header.present_chunk = 1;
			header.data_signs = htons(114514);
			header.type = DATA_TYPE::STRING;
			//header.chunk_total = infor_.size() / real_default;
			int chunks = infor_.size() / real_default;
			if (infor_.size() % real_default != 0) {
				chunks++;
			}
			header.chunk_total = chunks;
			size_t offest = 0;
			const char* top = infor_.data();
			size_t total = ntohl(header.total_size);
			SOCKET temp_sock = overlp->socket;
			
			if (header.chunk_total > 1) {
				ZeroMemory(&overlp->overlapped, sizeof(OVERLAPPED));
				
				overlp->id = htons(1);
				overlp->type = IO_TYPE::IO_SEND;
				overlp->wasBuf.buf = overlp->buffer;
				ZeroMemory(overlp->buffer, overlp->wasBuf.len);
				header.persent_size = htons(real_default);
				overlp->wasBuf.len = DEFAULT_BUFFER_SIZE;
				memcpy(overlp->wasBuf.buf, &header, sizeof(MengDataHeader));
				memcpy(overlp->wasBuf.buf + sizeof(MengDataHeader), top, real_default);
				

				size_t present = header.persent_size;
				present = ntohs(present);
				cout << "我的发我的发我的发" << endl;
				std::cout << "[Debug] sizeof(MengDataHeader): " << sizeof(MengDataHeader) << std::endl;
				std::cout << "[Debug] persent_size offset: " << offsetof(MengDataHeader, persent_size) << std::endl;
				std::cout << "[Debug] data_signs offset: " << offsetof(MengDataHeader, data_signs) << std::endl;

				cout.write(top + offest, present);


				WSASend(overlp->socket, &overlp->wasBuf, 1, &data_num, 0, &(overlp->overlapped), 0);
				offest += present;
				header.present_chunk++;

				while (offest < total) {
					//if (offest >= total) break;
					size_t present_real = real_default > (total - offest) ? (total - offest) : real_default;
					header.persent_size = 0;
					present = 0;
					header.persent_size = htons(present_real);
					present = header.persent_size;
					present = ntohs(header.persent_size);

					
					OverlappedPerIO* perio = new OverlappedPerIO;
					ZeroMemory(perio, sizeof(OverlappedPerIO));
					perio->id = htons(1145);
					perio->wasBuf.buf = perio->buffer;
					perio->wasBuf.len = present_real + sizeof(MengDataHeader);
					perio->socket = temp_sock;
					//ZeroMemory(&perio->buffer, perio->wasBuf.len);
					memset(&overlp->buffer, 0, sizeof(overlp->buffer));
					perio->type = IO_TYPE::IO_SEND;
					memcpy(perio->wasBuf.buf, &header, sizeof(MengDataHeader));
					memcpy(perio->wasBuf.buf + sizeof(MengDataHeader), top + offest, present_real);
					cout.write(top + offest, present_real);
					cout << "第二次正确数量:" << ntohs(header.persent_size) << endl;
					WSASend(perio->socket, &perio->wasBuf, 1, &data_num, 0, &(perio->overlapped), 0);
					offest += present_real;
					header.present_chunk++;
					
				}
				cout << "成功发送完所有数据..." << endl;
				break;
			}
			
			overlp->id = htons(1);
			header.persent_size = htons(infor_.size());
			ZeroMemory(&overlp->overlapped, sizeof(OVERLAPPED));
			memset(overlp->buffer, 0, sizeof(overlp->buffer));
			overlp->type = IO_TYPE::IO_SEND;
			overlp->wasBuf.len = DEFAULT_BUFFER_SIZE;
			memcpy(overlp->wasBuf.buf, &header, sizeof(MengDataHeader));
			memcpy(overlp->wasBuf.buf + sizeof(MengDataHeader),top,infor_.size());
			WSASend(overlp->socket, &overlp->wasBuf, 1, &data_num, 0, &(overlp->overlapped), 0);
			cout << "已成功按预定发送指定数据:" << overlp->buffer << endl;
		}
		break;
		case IO_TYPE::IO_SEND:
		{
			cout << "发送" << endl;
			cout << "发送数据量:" << bytesTrans << '\n';
			size_t uid = ntohl(overlp->id);
			/*if (uid == 114514) {
				delete overlp;
			}*/

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
	while (1) {
		if (mengServer.initServer(DEFAULT_PORT, pms) == false) {
			cerr << "初始化服务器/连接服务器失败,请尝试重新进行初始化连接..." << endl;
			closesocket(pms.listen_socket);
			CloseHandle(pms.iocp);
			system("pause");
			system("cls");
			continue;
		}
		break;
	}
	clog << "网络及通信初始化完毕 | the server network and listen socket initialization completed..." << '\n' << endl;
	int choies = 0;
	cout << "请输入\"1\"确认开启通信并接收数据" << endl;
	cin >> choies;
	
	switch (choies) {
	case 1:
		TempParamServerInfor temp = { &pms,&mengServer };
		auto func = std::bind(MwokerThread, &temp);
		for (int i = 0; i < DEFAULT_WOKER; i++) {
			CreateThread(NULL, 0, MwokerThread, &temp, 0, NULL);
		}
		mengServer.PostAccept(pms.listen_socket);
		
	}

	thread t1([] {
		while (1) {}
		});
	t1.join();
}