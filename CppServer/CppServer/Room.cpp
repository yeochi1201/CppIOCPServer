#include "Room.h"

Room::Room(Session* session) {
	InitializeCriticalSection(&session_cs);
	EnterRoom(session);
}

void Room::EnterRoom(Session* session) {
	EnterCriticalSection(&session_cs);
	session_list.push_back(session);
	LeaveCriticalSection(&session_cs);
}

void Room::LeaveRoom(Session* session) {
	EnterCriticalSection(&session_cs);
	session_list.remove(session);
	LeaveCriticalSection(&session_cs);

	if (session_list.empty())
		//TODO Delete Room
		;
}

void Room::SendMessage(char* pszParam, Session* session) {

}

void Room::SendFile(char* pszParam, Session* session) {

}