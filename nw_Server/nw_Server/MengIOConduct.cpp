#include "MengIOConduct.h"
bool MengIOConduct::recvive(SOCKET tcp_socket)
{
    if (!filesystem::exists("C:\\CanMeng_[Yang]_serverTest")) filesystem::create_directories("C:\\CanMeng_[Yang]_serverTest");
    ofstream writer("C:\\CanMeng_[Yang]_serverTest\\test.png", ios::binary | ios::out);
    if (!writer.is_open()) return false;
    string rec;
    memset(&buffer, 0, sizeof(buffer));
    while (1) {
        if (!MengTcpServer::recv(buffer, sizeof(buffer), tcp_socket)) {
            cout << "δ֪ԭ���ѶϿ�����,��������ʧ��..." << endl;
            writer.close();
            return false;
        }
        rec = buffer;
        if (!rec.compare("-data_end-")) break;
        writer.write(buffer, sizeof(buffer));
    }
    writer.close();
    return true;
}

bool MengIOConduct::sendBinaryFile(SOCKET tcp_socket, string path)
{
    memset(&buffer, 0, sizeof(buffer));
    ifstream reader(path);
    if (!reader.is_open()) return false;
    if ((filesystem::status(path).permissions() & filesystem::perms::owner_read) != filesystem::perms::none) {
        while (reader.read(buffer, sizeof(buffer))) {
            if (!MengTcpServer::send(buffer,sizeof(buffer), tcp_socket)) {
                cout << "δ֪ԭ���ѶϿ���,��������ʧ��..." << endl;
                return false;
            }
        }
        if (!MengTcpServer::send(buffer,sizeof(buffer), tcp_socket)) {
            cout << "���һ�����ݽ���ʧ��..." << endl;
            return false;
        }
    }
}
