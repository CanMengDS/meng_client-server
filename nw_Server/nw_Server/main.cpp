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
				cout << "�����쳣,��ʱ�ƹ�|�����ͻ���Ϊ:" << overlp->socket << endl;
				closesocket(overlp->socket);
				delete overlp;
				continue;
			}
			cout << "��ȡ��ɶ˿ڽ��ʧ��" << endl;
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
				cout << "�ͻ��˶Ͽ�:" << overlp->socket << endl;
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

				size_t offest = 0; //��¼��ȡ���ĸ�λ��
				const char* data_top = infor_.data(); //��ȡ�ַ�����C�ַ��������ʽ��ͷ��ַ
				const size_t data_size = infor_.size(); //��ȡ�ַ�����ռ���ֽ���
				size_t chunk_index = 0; //��ʼ����ǰ������Ƭ����Ϊ0(�·�header.present_partʱ��Ҫ�õ�)
				vector<vector<char>> temp_vec; //Ƕ�׵�vector�����ڹ�������
				while (offest < data_size) { //��ƫ��������data_sizeʱ˵���Ѿ����ַ����ָ��겢����Ƕ�׵�vector��

					header.present_chunk = chunk_index;					
					/*
					* �жϵ�ǰʵ�ʴ���vector���ֽ���,�����ͷ����ô���ݿ���С��1024���ͷ���С��(ʵ��ʣ�µ�)�����û�е�ͷ��ô1024�ͻ���С����˷���1024
					*/
					size_t real_remain = infor_.size() - offest;
					size_t data_present = real_default > real_remain ? real_remain : real_default;
					//vector<char> chunk(DEFAULT_BUFFER_SIZE); //��ʱ�����Ƭ���ݵĵط�
					memcpy(overlp->wasBuf.buf, &header, sizeof(MengDataHeader));
					memcpy(overlp->wasBuf.buf + sizeof(MengDataHeader), data_top + offest, data_present);
					
					//memcpy(chunk.data(), &header, sizeof(MengDataHeader));  //�������ͷ
					//memcpy(chunk.data() + sizeof(MengDataHeader), data_top + offest, data_present); //��ΪҪ��chunk������ͷ��ռ�ֽں�ʼ����������Ҫ������֮��
					//temp_vec.push_back(chunk);
					offest += data_present;
					chunk_index++;  //��ǰ��Ƭ����
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
				cout << "�ѳɹ���Ԥ������ָ������..." << endl;
			}

			
		}
		break;
		case IO_TYPE::IO_SEND:
		{
			cout << "��������:" << bytesTrans << endl;
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
		cerr << "����δ�ɹ�..." << endl;
	}
	clog << "���缰ͨ�ų�ʼ����� | the server network and listen socket initialization completed..." << '\n' << endl;
	int choies = 0;
	cout << "������\"1\"ȷ�Ͽ���ͨ�Ų���������" << endl;
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