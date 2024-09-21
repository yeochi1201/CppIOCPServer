#include "Listener.h"

Listener::Listener() {
	ResetWinsock();
	CreateSocket();
	BindPort();
	WaitingClient();
	AcceptClient();
}

Listener::~Listener() {
	CloseSocket();
}

bool Listener::ResetWinsock() {
	WSADATA wsa = { 0 };
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		puts("ERROR : Failed to Reset Window Socket");
		return false;
	}
	puts("Reset Window Socket");
	return true;
}

bool Listener::CreateSocket() {
	listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET) {
		puts("ERROR : Failed to Create Listen Socket");
		return false;
	}
	puts("Create Listener Socket");
	return true;
}

bool Listener::BindPort() {
	SOCKADDR_IN serverAddress = { 0 };
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(25000);
	serverAddress.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (::bind(listenSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
		puts("ERROR : Failed to bind Port");
		return false;
	}
	puts("Binding Port");
	return true;
}

bool Listener::WaitingClient() {
	if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		puts("ERROR : Failed to Listen");
		return false;
	}
	puts("Waiting Client");
	return true;
}

void Listener::AcceptClient() {
	SOCKADDR_IN clientAddress = { 0 };
	int nAddrLen = sizeof(clientAddress);
	SOCKET clientSocket = 0;
	char szBuffer[128] = { 0 };
	int nReceive = 0;

	while ((clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddress, &nAddrLen)) != INVALID_SOCKET) {
		puts("New Client Connected");
		fflush(stdout);

		while ((nReceive = ::recv(clientSocket, szBuffer, sizeof(szBuffer), 0)) > 0) {
			::send(clientSocket, szBuffer, sizeof(szBuffer), 0);
			puts(szBuffer);
			fflush(stdout);
			memset(szBuffer, 0, sizeof(szBuffer));
		}
	}
	ExitClient(&clientSocket);
}

void Listener::ExitClient(SOCKET* clientSocket) {
	::shutdown(*clientSocket, SD_BOTH);
	::closesocket(*clientSocket);
}

void Listener::CloseSocket() {
	puts("Close Listener Socket");
	::closesocket(listenSocket);
}