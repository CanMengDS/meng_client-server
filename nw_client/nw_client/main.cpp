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
		cerr << "δ�ɹ�����ͨѶ..." << endl;
		system("pause");
		return -1;
	}else{
		clog << "�ɹ�����ͨѶ..." << endl;
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
		cout << '\n' << "�����뷢������,���س���ȷ�Ϸ���..." << endl;
		cin >> user_document;
		memcpy(&send_buffer, user_document.c_str(), sizeof(user_document));
		if (mengClient.send(user_document.c_str(), tcp_socket) == false) {
			cerr << "δ֪����,δ�ܳɹ���������,�ѶϿ����������" << endl;
			break;
		}
		else {
			clog << "�ɹ���������" << endl;
		}

		if (mengClient.recv(recv_buffer, sizeof(recv_buffer), tcp_socket)) {
			cout << "��ȷ����" << endl;

			recvive_document = recv_buffer;
			cout << recv_buffer << endl;
			/*if (recvive_document.compare("|image|")) {
				conduct_.recvive(tcp_socket, mengClient);
			}
			else if (recvive_document.compare("-1")) {
				clog << "�������ȷ��Ӧ" << endl;
				continue;
			}*/
		}
	}
	
	system("pause");
}