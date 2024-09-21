#include "Connector.h"

Connector::Connector() {
	ResetWinsock();
	CreateSocket();
	BindPort();
	ChatMessage();
}

bool Connector::ResetWinsock() {
	WSADATA wsa = { 0 };
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		puts("ERROR : Failed to Reset Window Socket");
		return false;
	}
	puts("Reset Window Socket");
	return true;
}

bool Connector::CreateSocket() {
	clientSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET) {
		puts("ERROR : Failed to Create Socket");
		return false;
	}
	puts("Create Client Socket");
	return true;
}

bool Connector::BindPort() {
	SOCKADDR_IN serverAddress = { 0 };
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(25000);
	serverAddress.sin_addr.S_un.S_addr = inet_addr("192.168.0.2");
	if (::connect(clientSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
		puts("ERROR : Failed Connect Server");
		return false;
	}
	puts("Binding Port");
	return true;
}

void Connector::ChatMessage() {
	while (1) {
		gets_s(szBuffer);
		if (strcmp(szBuffer, "EXIT") == 0) break;
		::send(clientSocket, szBuffer, strlen(szBuffer) + 1, 0);
		memset(szBuffer, 0, sizeof(szBuffer));
		::recv(clientSocket, szBuffer, sizeof(szBuffer), 0);
		printf("From server : %s\n", szBuffer);
	}

	CloseSocket();
}

void Connector::CloseSocket() {
	puts("Close Client Socket");
	::closesocket(clientSocket);
	::WSACleanup();
}