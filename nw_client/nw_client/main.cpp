#pragma comment(lib, "Ws2_32.lib")
#include<iostream>
#include"MengTcpClient.h"
#include"IOconc.h"
using namespace std;

#define DEFAULT_BUFFER_SIZE 1024

int main() {
	//cout << argc << endl;
	//cout << argv[1] << "|" << argv[2] << endl;

	MengTcpClient mengClient;

	if (mengClient.tcp_connect("192.168.1.8",5408) == false) {
		cerr << "δ�ɹ�����ͨѶ..." << endl;
		system("pause");
		return -1;
	}else{
		clog << "�ɹ�����ͨѶ..." << endl;
	}

	char send_buffer[DEFAULT_BUFFER_SIZE];
	char recv_buffer[DEFAULT_BUFFER_SIZE];
	memset(&send_buffer, 0, sizeof(send_buffer));

	int cho = 0, num = 0;
	string user_document;

	SOCKET tcp_socket = mengClient.getClientSocket();
	MengDataHeader header;
	bool ismessage = false;
	while (1) {
		cout << '\n' << "�����뷢������,���س���ȷ�Ϸ���..." << endl;
		cin >> user_document;
		memcpy(&send_buffer, user_document.data(),user_document.size());
		if (mengClient.send(send_buffer,user_document.size(), tcp_socket) == false) {
			cerr << "δ֪����,δ�ܳɹ���������,�ѶϿ����������" << endl;
			break;
		}
		else {
			clog << "�ɹ���������" << endl;
		}

		if (mengClient.recv(recv_buffer, DEFAULT_BUFFER_SIZE, tcp_socket)) {
			cout << "��ȷ����" << endl;
			
			/*
			* ���ڿ�����
			* �ֶ�ʽģ���������
			*/
			memcpy(&header, recv_buffer, sizeof(MengDataHeader));
			if (header.chunk_total > 1) {
				int real_chunks = header.chunk_total--;

			}

			string goap(header.persent_size, '-');
			cout << goap << endl;
			cout << "�����������ֽ���Ϊ:" << header.total_size << '\n'
				<< "������������" << ((ismessage = header.type == DATA_TYPE::STRING) ? "string" : "δ֪") << endl;
			if (ismessage) {
				cout.write(recv_buffer + sizeof(MengDataHeader), header.total_size) << endl;
			}
			cout << goap << endl;
			memset(&header, 0, sizeof(MengDataHeader));
			memset(recv_buffer, 0, 1024);
			//cout << recv_buffer << endl;
		}
	}
	
	system("pause");
}