// CMNexus.cpp: 定义应用程序的入口点。
//

#include "../CMNexus/include/CMNexus.h"
#define DEFAULT_PORT 5408

int main()
{
	mutex temp_lock;
	condition_variable temp_cv;

	CMIOCPTcpServer cm_server;
	CMIOCPThreadPool cm_pool(4);
	ServerParams pms{ 0 };
	pms.server = &cm_server;

	while (1) {
		if (!cm_server.InitServer(DEFAULT_PORT, pms)) {
			closesocket(pms.sock);
			CloseHandle(pms.iocp);
			cerr << "初始化服务器错误..." << endl;
			system("pause");
			continue;
		}
		break;
	}

	function<void()> func = std::bind(&CMIOCPTcpServer::IOCPWokerThread, &cm_server, pms);
	for (int i = 0; i < 4; i++) {
		cm_pool.PostTask(func);
	}
	cm_server.PostAccept(pms.sock);

	thread t([&temp_cv,&temp_lock] {
		unique_lock<mutex> lock(temp_lock);
		temp_cv.wait(lock,[]{
			return false;
			});
		});
	t.join();

	cout << "Hello CMake." << endl;
	return 0;
}
