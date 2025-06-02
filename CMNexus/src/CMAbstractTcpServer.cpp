#pragma once
#include"../include/CMAbstractTcpServer.h"


CMAbstractTcpServer::CMAbstractTcpServer() {}

CMAbstractTcpServer::~CMAbstractTcpServer() {}

bool CMAbstractTcpServer::CloseListenSocket()
{
	closesocket(listen_socket);
	this->SetListenSocket(0);
	return true;
}

bool CMAbstractTcpServer::CLoseTcpServerSocket()
{
	closesocket(tcp_socket);
	this->SetTcpSocket(0);
	return true;
}

void CMAbstractTcpServer::SetListenSocket(SOCKET listen_socket)
{
	this->listen_socket = listen_socket;
}

void CMAbstractTcpServer::SetTcpSocket(SOCKET tcp_socket)
{
	this->tcp_socket = tcp_socket;
}

SOCKET CMAbstractTcpServer::GetListenSocket()
{
	return this->listen_socket;
}

SOCKET CMAbstractTcpServer::GetTcpSocket()
{
	return this->tcp_socket;
}
