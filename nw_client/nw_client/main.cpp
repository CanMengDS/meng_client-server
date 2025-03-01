#pragma comment(lib, "Ws2_32.lib")
#include<iostream>
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

	char buffer[1024];
	memset(&buffer, 0, sizeof(buffer));

	int cho = 0, num = 0;
	string user_document;
	while (1) {
		cout << '\n' << "请输入发送数据,并回车以确认发送..." << endl;
		cin >> user_document;
		memcpy(&buffer, user_document.c_str(), sizeof(user_document));
		if (mengClient.send(buffer) == false) {
			cerr << "未知错误,未能成功发送数据,已断开服务端连接" << endl;
			break;
		}
		else {
			clog << "成功发送数据" << endl;
		}
		num++;
	}
	
	system("pause");
}