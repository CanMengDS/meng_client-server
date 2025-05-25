#pragma pack(push, 1)
#pragma pack(pop)
#pragma comment(lib, "Ws2_32.lib")
#include<iostream>
#include"MengTcpClient.h"
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
	memset(&header, 0, sizeof(MengDataHeader));
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
			memset(&header, 0, sizeof(MengDataHeader));
			memcpy(&header, recv_buffer, sizeof(MengDataHeader));
			header.persent_size = ntohs(header.persent_size);
			size_t present_size = header.persent_size;
			cout << "���յ�������:" << present_size << endl;
			size_t total_size = ntohl(header.total_size);
			size_t signs = ntohs(header.data_signs);
			cout << "��������:string" << '\n' << "���ݴ�С:" << total_size << endl;
			if (header.chunk_total > 1) {
				std::cout << "[Debug] sizeof(MengDataHeader): " << sizeof(MengDataHeader) << std::endl;
				std::cout << "[Debug] persent_size offset: " << offsetof(MengDataHeader, persent_size) << std::endl;
				std::cout << "[Debug] data_signs offset: " << offsetof(MengDataHeader, data_signs) << std::endl;

				cout.write(recv_buffer + sizeof(MengDataHeader), present_size);
				size_t offest = present_size;
				int present_chunk = 1;
				//cout << "����Ϊ�����ļ���������������ģʽ..." << endl;
				while (offest < total_size) {
					
					memset(&header, 0, sizeof(MengDataHeader));
					if (!mengClient.recv(recv_buffer, DEFAULT_BUFFER_SIZE, tcp_socket)) break;
					//cout << "t" << endl;
					memcpy(&header, recv_buffer, sizeof(MengDataHeader));
					header.persent_size = ntohs(header.persent_size);
					present_size = header.persent_size;
					//present_size = ntohs(header.persent_size);
					cout.write(recv_buffer + sizeof(MengDataHeader), ntohs(header.persent_size));
					offest += present_size;
					present_chunk = header.present_chunk;
				}
				cout << '\n' << "�Ѿ���ɴ������ݽ���..." << endl;
				continue;
			}


			string goap(present_size, '-');
			cout << goap << endl;
			cout << "�����������ֽ���Ϊ:" << total_size << '\n'
				<< "������������" << ((ismessage = header.type == DATA_TYPE::STRING) ? "string" : "δ֪") << endl;
			if (ismessage) {
				cout.write(recv_buffer + sizeof(MengDataHeader), total_size) << endl;
			}
			cout << goap << endl;
			memset(&header, 0, sizeof(MengDataHeader));
			memset(recv_buffer, 0, 1024);
			}
	}
	system("pause");
}