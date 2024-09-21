#ifndef Connector_H
#define Connector_H

#include <WinSock2.h>
#include <stdio.h>
#include <tchar.h>
#include <sdkddkver.h>

#pragma comment (lib, "ws2_32")

class Connector {
public:
	Connector();
private:
	SOCKET clientSocket;
	char szBuffer[128] = { 0 };

	bool ResetWinsock();
	bool CreateSocket();
	bool BindPort();
	void ChatMessage();
	void CloseSocket();
};

#endif
