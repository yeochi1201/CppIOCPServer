#ifndef Listener_H
#define Listener_H

#include <WinSock2.h>
#include <stdio.h>
#include <tchar.h>
#include <sdkddkver.h>

#pragma comment(lib, "ws2_32")

class Listener {
public:
	Listener();
private:
	SOCKET listenSocket;
	
	bool ResetWinsock();
	bool CreateSocket();
	bool BindPort();
	bool WaitingClient();
	void AcceptClient();
	void ExitClient(SOCKET* clientSocket);
	void CloseSocket();
};
#endif