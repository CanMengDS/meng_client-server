#include "MengTcpClient.h"
bool MengTcpClient::tcp_connect(const char* in_server_ip, const unsigned short in_port)
{
    if (client_socket != INVALID_SOCKET)return false;
    server_ip = in_server_ip;
    cl_port = in_port;

    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    addrinfo* servaddr = nullptr;//存放服务器IP地址(大端序)的结构体的指针
    //memset(&servaddr, 0, sizeof(servaddr));
    if (getaddrinfo("192.168.1.7", "5408", &hints, &servaddr) != 0) {
        closesocket(client_socket);
        client_socket = -1;
        return false;
    }

    
    for (; servaddr != nullptr; servaddr = servaddr->ai_next) {
        if ((client_socket = socket(servaddr->ai_family, servaddr->ai_socktype, servaddr->ai_protocol)) == -1) continue;
        if (connect(client_socket, servaddr->ai_addr, servaddr->ai_addrlen) == 0) {
            break;
        }
        closesocket(client_socket);
        continue;
    }
    if (client_socket == -1) {
        return false;
    }

    //if (client_socket = socket(AF_INET, SOCK_STREAM, 0) == INVALID_SOCKET) return false;
    //sockaddr_in servaddr;
    //servaddr.sin_family = AF_INET;
    ///*servaddr.sin_port = htons(5408);
    //servaddr.sin_addr.s_addr = inet_addr("192.168.1.7");*/
    //servaddr.sin_port = htons(cl_port);
    //inet_pton(AF_INET, "192.168.1.7", &servaddr.sin_addr.s_addr);

    //if (connect(client_socket, (sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
    //    closesocket(client_socket);
    //    cout << "连接失败" << endl;
    //    return false;
    //}
    return true;
}

bool MengTcpClient::send(const char buffer[1024], SOCKET tcp_socket)
{
    if (client_socket == INVALID_SOCKET)return false;
    if (::send(tcp_socket, buffer,sizeof(buffer), 0) <= 0)return false;
    return true;
}

bool MengTcpClient::recv(char* buffer, const size_t buffer_len, SOCKET tcp_socket)
{
    memset(buffer, 0, buffer_len);
    int readn = ::recv(tcp_socket, buffer, buffer_len - 1, 0);
    if (readn <= 0) {
        memset(buffer, 0, buffer_len);
        return false;
    }
    cout << "现在为此，buffer为:" << buffer << endl;
    return true;
}

SOCKET MengTcpClient::getClientSocket()
{
    return client_socket;
}

MengTcpClient::MengTcpClient():client_socket(-1)
{

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        throw runtime_error("网络初始化失败");
    }
}

MengTcpClient::~MengTcpClient()
{
}
