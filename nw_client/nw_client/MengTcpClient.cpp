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
    if (getaddrinfo("192.168.1.8", "5408", &hints, &servaddr) != 0) {
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

bool MengTcpClient::send(const char* buffer, size_t len,SOCKET tcp_socket)
{
    if (client_socket == INVALID_SOCKET)return false;
    if (::send(tcp_socket, buffer,len, 0) <= 0)return false;
    return true;
}

bool MengTcpClient::recv(char* buffer, const size_t buffer_len, SOCKET tcp_socket)
{
    memset(buffer, 0, buffer_len);
    int readn = ::recv(tcp_socket, buffer, buffer_len, 0);
    if (readn <= 0) {
        memset(buffer, 0, buffer_len);
        return false;
    }
    //cout << "现在为此，buffer为:" << buffer << endl;
    return true;
}

bool MengTcpClient::recvLargeData(char* first_data, const size_t first_len, const MengDataHeader* header, SOCKET socket)
{
    //没有判断数据类型，假定数据类型一直为string
    if (first_len != 1024) return false;

    char* recv_buffer = first_data;
    int chunks = header->chunk_total;
    int present = 1;
    uint32_t total_size = header->total_size;
    uint32_t recv_offest = 0;

    cout.write(recv_buffer, first_len);
    
    memset(recv_buffer, 0, first_len);

    while (recv_offest < total_size) {
        if (this->recv(recv_buffer, 1024, socket)) {
            cout << "大型文件-正确接收:" << header->present_chunk << endl;
            recv_offest += header->persent_size;
            if (header->present_chunk == header->chunk_total) break;
        }
    }
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
