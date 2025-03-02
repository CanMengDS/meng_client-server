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
		cerr << "连接未成功..." << endl;
	}
	int choies = 0;
	cout << "请输入\"1\"来确定接收数据" << endl;
	cin >> choies;
	
	switch (choies) {
	case 1:
		int accept_num = 0;
			/*pair<int, bool> re_detection = mengServer.accept();
			if (!re_detection.second) {
				cerr << "与客户端连接错误，已跳过此次连接..." << endl;
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
					cout << "连接已断开" << endl;
					break;
				}
				cout << "数据接收 :" << buffer << endl;
			}*/





			//string buffer;
			//while (1) {
			//	if (mengServer.recv(buffer, 1024) == false) {
			//		cout << "客户端已断开连接..." << endl;
			//		break;
			//	}
			//	else {
			//		cout << "成功接收数据" << endl;
			//		cout << "数据为：" << buffer << '\n';
			//	}

			//	/*buffer = "Hello netWork,I'm going to here! |  from Server";
			//	if (mengServer.send(buffer) == false) {
			//		cout << "客户端发送信息发生错误" << endl;
			//	}
			//	cout << "发送了:" << buffer << endl;*/
			//}
	}
}