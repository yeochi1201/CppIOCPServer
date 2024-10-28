#ifndef Listener_H
#define Listener_H

#include <stdio.h>
#include <tchar.h>
#include <winsock2.h>
#include <SDKDDKVer.h>
#pragma comment(lib, "ws2_32")

#include <list>
#include <iterator>
#include<Windows.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS

class Listener {
public:
	Listener();
	void SendChattingMessage(char* pszParam, SOCKET clientSocket);
private:
	bool ResetWinsock();
	bool CreateSocket();
	bool BindPort();
	bool AddClientSocket(SOCKET clientSocket);
	bool InitCtrlHandler();
	bool WaitingClient(SOCKET listenSocket);
	SOCKET AcceptClient();
	void ObserveClient();
};
#endif