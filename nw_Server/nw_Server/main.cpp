#pragma comment(lib, "Ws2_32.lib")
#include<iostream>
#include <cstdlib>
#include <cstring>
#include <thread>
#include"MengMultiTcpConduct.h"
#include"MengTcpServer.h"
using namespace std;

int main(int argc, char argv[]) {
	MengTcpServer mengServer;
	if ( mengServer.initServer(atoi(argv)) == false) {
		cerr << "����δ�ɹ�..." << endl;
	}
	int choies = 0;
	cout << "������\"1\"��ȷ����������" << endl;
	cin >> choies;
	
	switch (choies) {
	case 1:
		int accept_num = 0;
			/*pair<int, bool> re_detection = mengServer.accept();
			if (!re_detection.second) {
				cerr << "��ͻ������Ӵ����������˴�����..." << endl;
				continue;
			}
			int exchange_socket = re_detection.first;
			accept_num++;*/
		thread t([&mengServer] {
			MengMultiTcpConduct conduct;
			const SOCKET listen = mengServer.getServerListenSocket();
			conduct.initExchange(listen);
			});
		
		t.join();
		string buffer;

			


			/*while (1) {
				if (!mengServer.recv(buffer, 1024,re_detection.first)) {
					cout << "�����ѶϿ�" << endl;
					break;
				}
				cout << "���ݽ��� :" << buffer << endl;
			}*/





			//string buffer;
			//while (1) {
			//	if (mengServer.recv(buffer, 1024) == false) {
			//		cout << "�ͻ����ѶϿ�����..." << endl;
			//		break;
			//	}
			//	else {
			//		cout << "�ɹ���������" << endl;
			//		cout << "����Ϊ��" << buffer << '\n';
			//	}

			//	/*buffer = "Hello netWork,I'm going to here! |  from Server";
			//	if (mengServer.send(buffer) == false) {
			//		cout << "�ͻ��˷�����Ϣ��������" << endl;
			//	}
			//	cout << "������:" << buffer << endl;*/
			//}
	}
}