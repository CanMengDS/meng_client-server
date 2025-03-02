#pragma comment(lib, "Ws2_32.lib")
#include<iostream>
#include"MengIOConduct.h"
#include"MengTcpClient.h"
using namespace std;

int main(int argc, char* argv[]) {
	//cout << argc << endl;
	//cout << argv[1] << "|" << argv[2] << endl;
	if (argc != 3) {
		return -1;
	}

	MengTcpClient mengClient;

	if (mengClient.tcp_connect(argv[1], atoi(argv[2])) == false) {
		cerr << "未成功建立通讯..." << endl;
		system("pause");
		return -1;
	}else{
		clog << "成功建立通讯..." << endl;
	}

	char send_buffer[1024];
	char recv_buffer[1024];
	memset(&send_buffer, 0, sizeof(send_buffer));

	int cho = 0, num = 0;
	string user_document;

	SOCKET tcp_socket = mengClient.getClientSocket();
	string recvive_document;
	MengIOConduct conduct_;
	while (1) {
		cout << '\n' << "请输入发送数据,并回车以确认发送..." << endl;
		cin >> user_document;
		memcpy(&send_buffer, user_document.c_str(), sizeof(user_document));
		if (mengClient.send(user_document.c_str(), tcp_socket) == false) {
			cerr << "未知错误,未能成功发送数据,已断开服务端连接" << endl;
			break;
		}
		else {
			clog << "成功发送数据" << endl;
		}

		if (mengClient.recv(recv_buffer, sizeof(recv_buffer), tcp_socket)) {
			cout << "正确接收" << endl;

			recvive_document = recv_buffer;
			cout << recv_buffer << endl;
			/*if (recvive_document.compare("|image|")) {
				conduct_.recvive(tcp_socket, mengClient);
			}
			else if (recvive_document.compare("-1")) {
				clog << "服务端正确回应" << endl;
				continue;
			}*/
		}
	}
	
	system("pause");
}