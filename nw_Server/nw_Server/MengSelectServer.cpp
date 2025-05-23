#include "MengSelectServer.h"

MengSelectServer::MengSelectServer(){
	FD_ZERO(&read_set); 
	FD_ZERO(&temp_read_set);
	memset(&buffer, 0, sizeof(buffer));
}

SOCKET MengSelectServer::initExchange(SOCKET server_listen_socket) {
	FD_SET(server_listen_socket, &read_set); 
	sockaddr_in client_dm;
	int client_dm_len = sizeof(client_dm);
	char client_ip[INET_ADDRSTRLEN];
	string docm;
	string tempDom;
	while (1) {
		temp_read_set = read_set;
		int ret = select(-1, &temp_read_set, NULL, NULL, NULL);
		if (FD_ISSET(server_listen_socket, &temp_read_set)) {
			SOCKET tcp_socket = ::accept(server_listen_socket, (sockaddr*)&client_dm, &client_dm_len);
			FD_SET(tcp_socket, &read_set);
			inet_ntop(AF_INET, &client_dm.sin_addr.S_un.S_addr, client_ip, sizeof(client_ip));
			client_ips.emplace_back(client_ip);
			clog << "有新的客户端接入:" << client_ip << "|" << client_dm.sin_port << endl;
		}

		
		for (int i = 0; i < temp_read_set.fd_count;i++) {
			SOCKET sock = temp_read_set.fd_array[i];
			if (sock != server_listen_socket) {
				if (sock == INVALID_SOCKET) {
					cout << "已有客户端断开连接..." << endl;
					FD_CLR(sock, &read_set);
					closesocket(sock);
					continue;
				}

				if (MengTcpServer::recv(buffer,sizeof(buffer), sock)) {
					docm = buffer;
					/*if (docm.compare("123")) {
						tempDom = "|image|";
						memset(buffer_t, 0, sizeof(buffer_t));
						memcpy(buffer_t, &tempDom, sizeof(tempDom));
						if (MengTcpServer::send(buffer_t, sizeof(buffer_t), sock)) {
							io_conduct.sendBinaryFile(sock, "‪D:\\Desktop\\wall.png");
						}
					}*/
					cout << "[新的消息*" << sock << "]:" << buffer << endl;
				}
				else {
					cerr << "消息接收失败,客户端已断开" << endl;
					FD_CLR(sock, &read_set);
					closesocket(sock);
					continue;
				}
				tempDom = "[null]";
				strncpy_s(buffer,sizeof(buffer),tempDom.c_str(), sizeof(tempDom));
				//memcpy(buffer, tempDom.c_str(), sizeof(tempDom));
				if (!MengTcpServer::send(buffer, sizeof(buffer), sock)) {
					cout << "未知错误,发送数据失败..." << endl;
				}
				else {
					cout << "正确返回数据给客户端:" << buffer << endl;
				}
			}
		}
	}
}
