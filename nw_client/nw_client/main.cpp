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
		cerr << "未成功建立通讯..." << endl;
		system("pause");
		return -1;
	}else{
		clog << "成功建立通讯..." << endl;
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
		cout << '\n' << "请输入发送数据,并回车以确认发送..." << endl;
		cin >> user_document;
		memcpy(&send_buffer, user_document.data(),user_document.size());
		if (mengClient.send(send_buffer,user_document.size(), tcp_socket) == false) {
			cerr << "未知错误,未能成功发送数据,已断开服务端连接" << endl;
			break;
		}
		else {
			clog << "成功发送数据" << endl;
		}
		if (mengClient.recv(recv_buffer, DEFAULT_BUFFER_SIZE, tcp_socket)) {
			cout << "正确接收" << endl;
			memset(&header, 0, sizeof(MengDataHeader));
			memcpy(&header, recv_buffer, sizeof(MengDataHeader));
			header.persent_size = ntohs(header.persent_size);
			size_t present_size = header.persent_size;
			cout << "接收到数据量:" << present_size << endl;
			size_t total_size = ntohl(header.total_size);
			size_t signs = ntohs(header.data_signs);
			cout << "数据类型:string" << '\n' << "数据大小:" << total_size << endl;
			if (header.chunk_total > 1) {
				std::cout << "[Debug] sizeof(MengDataHeader): " << sizeof(MengDataHeader) << std::endl;
				std::cout << "[Debug] persent_size offset: " << offsetof(MengDataHeader, persent_size) << std::endl;
				std::cout << "[Debug] data_signs offset: " << offsetof(MengDataHeader, data_signs) << std::endl;

				cout.write(recv_buffer + sizeof(MengDataHeader), present_size);
				size_t offest = present_size;
				int present_chunk = 1;
				//cout << "类型为大型文件，开启大型数据模式..." << endl;
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
				cout << '\n' << "已经完成大型数据接收..." << endl;
				continue;
			}


			string goap(present_size, '-');
			cout << goap << endl;
			cout << "接收总数据字节数为:" << total_size << '\n'
				<< "接收数据类型" << ((ismessage = header.type == DATA_TYPE::STRING) ? "string" : "未知") << endl;
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