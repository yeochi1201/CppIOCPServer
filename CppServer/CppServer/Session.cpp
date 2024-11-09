#include "Session.h"

void Session::Login(std::string userID, SOCKET clientSocket) {
	this->userID = userID;
	this->clientSocket = clientSocket;
}

void Session::Logout() {
	room->LeaveRoom(this);
	room = NULL;
}

void Session::ChangeRoom(Room* room) {
	if (this->room != NULL) {
		this->room->LeaveRoom(this);
	}
	room->EnterRoom(this);
	this->room = room;
}