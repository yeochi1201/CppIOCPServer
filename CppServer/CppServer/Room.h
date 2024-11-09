#ifndef Room_H
#define Room_H

#include <stdio.h>
#include<tchar.h>
#include <WinSock2.h>
#include <sdkddkver.h>
#pragma comment(lib, "ws2_32")

#include <string>
#include <list>

#include "Session.h"

class Room {
public:
	int roomID;
	std::list<Session*> session_list;
	CRITICAL_SECTION session_cs;
	SOCKET listen_socket;

	Room(Session* session);

	void EnterRoom(Session* session);
	void LeaveRoom(Session* session);
private:
	void SendMessage(char* pszParam, Session* session);
	void SendFile(char* pszParam, Session* session);
};
#endif
