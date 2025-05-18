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

	while (1) {
		bool result = GetQueuedCompletionStatus(completionPort, &bytesTrans, &Key, (LPOVERLAPPED*)&overlp, INFINITE);
		if (!result) {
			if ((GetLastError() == WAIT_TIMEOUT) || (GetLastError() == ERROR_NETNAME_DELETED)) {
				cout << "�����쳣,��ʱ�ƹ�|�����ͻ���Ϊ:" << overlp->socket << endl;
				CancelIoEx((HANDLE)overlp->socket, &overlp->overlapped);
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
			mengServer->PostAccept(pms->listen_socket);
			
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
			string infor_k = "Nowadays, there is a increasing number for people to join in the activities of volunteering. It��s well-known that volunteering job is making contribution to the society with nothing in return. It��s like the Leifeng spirit. However, knowing this, there are still many people would like to devote themseves in this trend. For instance, the 20xx Beijing Olympic Games has so many volunteers. They made lots of contribution to the success of opening the Olympics. Moreover, we can often see that there many students go to gerocomium to visit the old to bring warm to the lonely people. Even there are some people��s career is volunteer. They are ready to help others. Although the volunteers are much more common, we still need to thanks them to admire them devoting themseves to the society.After dinner, we enjoy the Spring Festival Gala with great interest, but the most interesting thing is magic. At the beginning of the stairs, there were fireworks, and my father and I came to the balcony to see the fireworks. In the sky, the fireworks were colorful and varied. It was really beautiful! Fireworks are very beautiful, I hope you can consciously stop fireworks, for the environment for our survival can be better! Imperceptibly, television began to greet the new year countdown, I fell asleep in a happy daze, wake up on the second day, he heard the sound of firecrackers, my mother said I grow old. The most happy thing is that there is a red bag under the pillow. Ha ha...";
			size_t real_default = DEFAULT_BUFFER_SIZE - sizeof(MengDataHeader);

			MengDataHeader header;
			header.chunk_total = infor_.size() / (DEFAULT_BUFFER_SIZE - sizeof(MengDataHeader));
			if (infor_.size() / (DEFAULT_BUFFER_SIZE - sizeof(MengDataHeader) % (DEFAULT_BUFFER_SIZE - (sizeof(MengDataHeader))) != 0)) {
				header.chunk_total++;
			}
			header.total_size = htonl(infor_.size());
			header.present_chunk = 1;
			header.data_signs = htonl(1);
			header.type = DATA_TYPE::STRING;

			
			if (infor_.size() > (DEFAULT_BUFFER_SIZE - sizeof(MengDataHeader))) {

				size_t offest = 0; //��¼��ȡ���ĸ�λ��
				vector<OverlappedPerIO*> chunk_context;
				//header.data_signs = htonl(mengServer->getRandomFileSign(10));
				header.data_signs = htonl(666);

			//	while (1) {

			//		const char* data_top = infor_.data(); //��ȡ�ַ�����C�ַ��������ʽ��ͷ��ַ
			//		const size_t data_size = infor_.size(); //��ȡ�ַ�����ռ���ֽ���
			//		size_t chunk_index = 1; //��ʼ����ǰ������Ƭ����Ϊ0(�·�header.present_partʱ��Ҫ�õ�)
			//		vector<vector<char>> temp_vec; //Ƕ�׵�vector�����ڹ�������
			//		while (offest < data_size) { //��ƫ��������data_sizeʱ˵���Ѿ����ַ����ָ��겢����Ƕ�׵�vector��

			//			header.present_chunk = chunk_index;
			//			/*
			//			* �жϵ�ǰʵ�ʴ���vector���ֽ���,�����ͷ����ô���ݿ���С��1024���ͷ���С��(ʵ��ʣ�µ�)�����û�е�ͷ��ô1024�ͻ���С����˷���1024
			//			*/
			//			size_t real_remain = infor_.size() - offest;
			//			size_t data_present = real_default > real_remain ? real_remain : real_default;
			//			
			//			ZeroMemory(&overlp->overlapped, sizeof(OVERLAPPED));
			//			memset(overlp->buffer, 0, sizeof(overlp->buffer));
			//			overlp->type = IO_TYPE::IO_SEND;
			//			memcpy(overlp->wasBuf.buf, &header, sizeof(MengDataHeader));
			//			memcpy(overlp->wasBuf.buf + sizeof(MengDataHeader), data_top + offest, data_present);
			//			//WSASend(overlp->socket, &overlp->wasBuf, 1, &data_num, 0, &(overlp->overlapped), 0);

			//			cout.write(data_top + offest, data_present) << '\n' << 'n';
			//			int result = WSASend(overlp->socket, &overlp->wasBuf, 1, &data_num, 0, &(overlp->overlapped), 0);
			//			if (result == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING) {
			//				CancelIoEx((HANDLE)overlp->socket, &overlp->overlapped);
			//				closesocket(overlp->socket);
			//				delete overlp;
			//				break;
			//			}
			//			offest += data_present;
			//			chunk_index++;  //��ǰ��Ƭ����
			//	}
			//		cout << "�Ѿ���Ԥ����������" << endl;
			//		break;
			//}
			}else{
				header.persent_size = ntohl(header.total_size);
				ZeroMemory(&overlp->overlapped, sizeof(OVERLAPPED));
				memset(overlp->buffer, 0, sizeof(overlp->buffer));
				overlp->type = IO_TYPE::IO_SEND;
				memcpy(overlp->wasBuf.buf, &header, sizeof(MengDataHeader));
				memcpy(overlp->wasBuf.buf + sizeof(MengDataHeader),infor_.data(),infor_.size());
				//overlp->wasBuf.buf = infor_.data();
				//overlp->wasBuf.len = infor_.size();
				WSASend(overlp->socket, &overlp->wasBuf, 1, &data_num, 0, &(overlp->overlapped), 0);
				cout << "�ѳɹ���Ԥ������ָ������:" << overlp->buffer << endl;
			}

			
		}
		break;
		case IO_TYPE::IO_SEND:
		{
			cout << "����������:" << bytesTrans << '\n';
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
			cerr << "��ʼ��������/���ӷ�����ʧ��,�볢�����½��г�ʼ������..." << endl;
			closesocket(pms.listen_socket);
			CloseHandle(pms.iocp);
			system("pause");
			system("cls");
			continue;
		}
		break;
	}
	clog << "���缰ͨ�ų�ʼ����� | the server network and listen socket initialization completed..." << '\n' << endl;
	int choies = 0;
	cout << "������\"1\"ȷ�Ͽ���ͨ�Ų���������" << endl;
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