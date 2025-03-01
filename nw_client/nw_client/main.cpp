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
		cerr << "δ�ɹ�����ͨѶ..." << endl;
		system("pause");
		return -1;
	}else{
		clog << "�ɹ�����ͨѶ..." << endl;
	}

	char buffer[1024];
	memset(&buffer, 0, sizeof(buffer));

	int cho = 0, num = 0;
	string user_document;
	while (1) {
		cout << '\n' << "�����뷢������,���س���ȷ�Ϸ���..." << endl;
		cin >> user_document;
		memcpy(&buffer, user_document.c_str(), sizeof(user_document));
		if (mengClient.send(buffer) == false) {
			cerr << "δ֪����,δ�ܳɹ���������,�ѶϿ����������" << endl;
			break;
		}
		else {
			clog << "�ɹ���������" << endl;
		}
		num++;
	}
	
	system("pause");
}