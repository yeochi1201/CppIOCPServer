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

class Listener {
public:
	Listener();
	void SendChattingMessage(char* pszParam);
private:
	bool ResetWinsock();
	bool CreateSocket();
	bool BindPort();
	bool AddClientSocket(SOCKET clientSocket);
	bool InitCtrlHandler();
	bool WaitingClient();
	void AcceptClient();
};
#endif