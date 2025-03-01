#include "MengMultiTcpConduct.h"

MengMultiTcpConduct::MengMultiTcpConduct(){ 
	FD_ZERO(&read_set); 
	FD_ZERO(&temp_read_set);
	memset(&buffer, 0, sizeof(buffer));
}

SOCKET MengMultiTcpConduct::initExchange(SOCKET server_listen_socket, function<bool(string& buffer, const size_t maxlen, int tcp_socket)> recv_document_function) {
	FD_SET(server_listen_socket, &read_set); 
	sockaddr_in client_dm;
	int client_dm_len = sizeof(client_dm);
	char client_ip[INET_ADDRSTRLEN];
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
				if (recv_document_function(buffer, 1024, sock)) {
					cout << "[新的消息]:" << buffer << endl;
				}
				else {
					cerr << "消息接收失败,客户端已断开" << endl;
					FD_CLR(sock, &read_set);
					closesocket(sock);
					continue;
				}

			}
		}
	}
}
