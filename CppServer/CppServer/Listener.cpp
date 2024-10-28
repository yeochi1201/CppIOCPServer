#include "Listener.h"

SOCKET listenSocket;
std::list<SOCKET> socket_list;
Listener* pListener;

Listener::Listener() {
	pListener = this;
	ResetWinsock();
	CreateSocket();
	BindPort();
	WaitingClient(listenSocket);
	ObserveClient();
}

bool CloseSocketHandler(DWORD dwType) {
	if (dwType == CTRL_C_EVENT) {
		std::list<SOCKET>::iterator it;
		::shutdown(listenSocket, SD_BOTH);

		for (it = socket_list.begin(); it != socket_list.end(); it++) {
			::closesocket(*it);
		}
		socket_list.clear();

		puts("Socket Shutdown complete");
		::closesocket(listenSocket);

		::WSACleanup();
		exit(0);
		return TRUE;
	}
	return FALSE;
}
bool Listener::InitCtrlHandler() {
	if (::SetConsoleCtrlHandler((PHANDLER_ROUTINE)CloseSocketHandler, TRUE) == FALSE) {
		puts("ERROR : Ctrl Handler Setting Failed");
	}
	return TRUE;
}

bool Listener::AddClientSocket(SOCKET clientSocket) {
	socket_list.push_back(clientSocket);
	puts("New Client Come In");
	return TRUE;
}

void Listener::SendChattingMessage(char* pszParam, SOCKET clientSocket) {
	int msgLength = strlen(pszParam);
	std::list<SOCKET>::iterator it;

	for (it = socket_list.begin(); it != socket_list.end(); ++it) {
		if (*it != clientSocket && *it != listenSocket) {
			::send(*it, pszParam, sizeof(char) * (msgLength + 1), 0);
		}
	}
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
	InitCtrlHandler();
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

bool Listener::WaitingClient(SOCKET listenSocket) {
	if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		puts("ERROR : Failed to Listen");
		return false;
	}
	puts("Waiting Client");
	socket_list.push_back(listenSocket);
	return true;
}

SOCKET Listener::AcceptClient() {
	SOCKADDR_IN clientAddress = { 0 };
	int nAddrLen = sizeof(clientAddress);
	SOCKET clientSocket = 0;

	if((clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddress, &nAddrLen)) != INVALID_SOCKET) {
		if (!AddClientSocket(clientSocket)) {
			puts("ERROR : No More Accept Client");
			return NULL;
		}
	}
	return clientSocket;
}

void Listener::ObserveClient() {
	UINT nCount;
	FD_SET fdRead;
	std::list<SOCKET>::iterator it;
	
	puts("Start IO MultiPlexing Chatting Server");
	do {
		FD_ZERO(&fdRead);
		for (it = socket_list.begin(); it != socket_list.end(); it++) {
			FD_SET(*it, &fdRead);
		}

		::select(0, &fdRead, NULL, NULL, NULL);

		nCount = fdRead.fd_count;
		for (int idx = 0; idx < nCount; idx++) {
			if(!FD_ISSET(fdRead.fd_array[idx], &fdRead))
				continue;

			if (fdRead.fd_array[idx] == listenSocket) {
				FD_SET(AcceptClient(), &fdRead);
			}
			else {
				char buffer[1024] = { 0 };
				int receive = ::recv(fdRead.fd_array[idx], buffer, sizeof(buffer), 0);
				if (receive < 0) {
					::closesocket(fdRead.fd_array[idx]);
					FD_CLR(fdRead.fd_array[idx], &fdRead);
					socket_list.remove(fdRead.fd_array[idx]);
					puts("Client Disconnect");
				}
				else
					puts(buffer);
					SendChattingMessage(buffer, fdRead.fd_array[idx]);
			}
		}
	} while (listenSocket != NULL);
}