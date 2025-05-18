#pragma comment(lib, "Ws2_32.lib")
#include<iostream>
#include <vector>
#include<algorithm>
#include"MengTcpClient.h"
#include"IOconc.h"
#include <random>
using namespace std;

#define DEFAULT_BUFFER_SIZE 1024

int main() {

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
	UnTreatedLargeFile large_file;

	vector<int> random_base(100);
	for (int i = 1; i <= 100; i++) {
		random_base.push_back(i);
	}
	vector<int> random_out(3,0);

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
		cout << "666" << endl;
			if (mengClient.recv(recv_buffer, DEFAULT_BUFFER_SIZE - sizeof(MengDataHeader), tcp_socket)) {
				cout << "��ȷ����" << endl;

				/*
				* ���ڿ�����
				* �ֶ�ʽģ���������
				*/

				memcpy(&header, recv_buffer, sizeof(MengDataHeader));

				if (header.chunk_total > 1 && !large_file.is_real) {
					large_file.is_real = true;
					large_file.file_signs = ntohl(header.data_signs);
					large_file.is_large = LARGE_FILE_SIGN::IS_LARGE_FILE;
				}
				cout << "is" << endl;
				
				/*if (large_file.is_large == LARGE_FILE_SIGN::IS_LARGE_FILE) {
					switch (header.type) {
					case DATA_TYPE::STRING: {
						if (ntohl(large_file.file_signs) == 0 && !large_file.is_real) break;
						if (header.data_signs != large_file.file_signs)break;
						cout.write(recv_buffer + sizeof(MengDataHeader), header.persent_size);
						if (header.chunk_total == header.present_chunk)break;
					}
						break;
					case DATA_TYPE::JPG:
						break;
					case DATA_TYPE::TXT:
						break;
					}
				}
				else {*/
					string goap(header.persent_size, '-');
					cout << goap << endl;
					cout << "�����������ֽ���Ϊ:" << ntohl(header.total_size) << '\n'
						<< "������������" << ((ismessage = header.type == DATA_TYPE::STRING) ? "string" : "δ֪") << endl;
					if (ismessage) {
						cout.write(recv_buffer + sizeof(MengDataHeader), ntohl(header.total_size)) << endl;
					}
					cout << goap << endl;
					memset(&header, 0, sizeof(MengDataHeader));
					memset(recv_buffer, 0, 1024);
					
				//}
		}
	}
	
	system("pause");
}