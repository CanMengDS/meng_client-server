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
			
			/*
			* 正在开发中
			* 分段式模块接收数据
			*/
			memcpy(&header, recv_buffer, sizeof(MengDataHeader));
			if (header.chunk_total > 1) {
				int real_chunks = header.chunk_total--;

			}

			string goap(header.persent_size, '-');
			cout << goap << endl;
			cout << "接收总数据字节数为:" << header.total_size << '\n'
				<< "接收数据类型" << ((ismessage = header.type == DATA_TYPE::STRING) ? "string" : "未知") << endl;
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