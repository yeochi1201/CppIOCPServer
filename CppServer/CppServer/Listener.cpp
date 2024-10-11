#include "Listener.h"

SOCKET listenSocket;
std::list<SOCKET> client_list;
CRITICAL_SECTION client_cs;
Listener* pListener;

Listener::Listener() {
	pListener = this;
	ResetWinsock();
	CreateSocket();
	BindPort();
	WaitingClient();
	AcceptClient();
}

bool CloseSocketHandler(DWORD dwType) {
	if (dwType == CTRL_C_EVENT) {
		std::list<SOCKET>::iterator it;
		::shutdown(listenSocket, SD_BOTH);

		::EnterCriticalSection(&client_cs);
		for (it = client_list.begin(); it != client_list.end(); it++) {
			::closesocket(*it);
		}
		client_list.clear();

		puts("Socket Shutdown complete");
		::DeleteCriticalSection(&client_cs);
		::closesocket(listenSocket);

		::WSACleanup();
		exit(0);
		return TRUE;
	}
	return FALSE;
}

DWORD WINAPI ThreadFunction(LPVOID nParam) {
	char szBuffer[128] = { 0 };
	int nReceive = 0;
	SOCKET clientSocket = (SOCKET)nParam;
	puts("New Client Connected");

	while ((nReceive = ::recv(clientSocket, szBuffer, sizeof(szBuffer), 0)) > 0) {
		::send(clientSocket, szBuffer, sizeof(szBuffer), 0);
		puts(szBuffer);
		pListener->SendChattingMessage(szBuffer);
		memset(szBuffer, 0, sizeof(szBuffer));
	}

	::EnterCriticalSection(&client_cs);
	client_list.remove(clientSocket);
	::LeaveCriticalSection(&client_cs);

	puts("Client Disconnect");
	::shutdown(clientSocket, SD_BOTH);
	::closesocket(clientSocket);

	return 0;
}

bool Listener::InitCtrlHandler() {
	if (::SetConsoleCtrlHandler((PHANDLER_ROUTINE)CloseSocketHandler, TRUE) == FALSE) {
		puts("ERROR : Ctrl Handler Setting Failed");
	}
	return TRUE;
}

bool Listener::AddClientSocket(SOCKET clientSocket) {
	::EnterCriticalSection(&client_cs);
	client_list.push_back(clientSocket);
	::LeaveCriticalSection(&client_cs);
	puts("New Client Comein");
	return TRUE;
}

void Listener::SendChattingMessage(char* pszParam) {
	int msgLength = strlen(pszParam);
	std::list<SOCKET>::iterator it;
	
	::EnterCriticalSection(&client_cs);
	for (it = client_list.begin(); it != client_list.end(); it++) {
		::send(*it, pszParam, sizeof(char) * (msgLength + 1), 0);
	}
	::LeaveCriticalSection(&client_cs);
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
	::InitializeCriticalSection(&client_cs);

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
	DWORD dwThreadID = 0;
	HANDLE clientThread;

	while ((clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddress, &nAddrLen)) != INVALID_SOCKET) {
		clientThread = ::CreateThread(NULL, 0, ThreadFunction, (LPVOID)clientSocket, 0, &dwThreadID);
		::CloseHandle(clientThread);
	}
}