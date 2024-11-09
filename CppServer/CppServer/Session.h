#ifndef Session_H
#define Session_H

#include <stdio.h>
#include<tchar.h>
#include <WinSock2.h>
#include <sdkddkver.h>
#pragma comment(lib, "ws2_32")

#include <string>

#include "Room.h"

class Session {
public:
	std::string userID;
	SOCKET clientSocket;
	Room* room; //current connected room

	void Login(std::string userID, SOCKET clientSocket);
	void Logout();
private:
	void ChangeRoom(Room* room);
};

#endif