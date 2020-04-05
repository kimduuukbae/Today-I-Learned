#pragma once
#include <WinSock2.h>
#include "Packet.h"
#include <queue>
#pragma comment(lib, "ws2_32")

class ServerDevice {
public:
	bool initialize(const char* ip, int port, HWND winHandle);
	void destroy();
	PlayerInfo recvInfo();
	void sendKey(const E_PACKET_TYPE& keyButton, int id);
private:
	SOCKET serverSocket;
	sockaddr_in serverAddr;
	WSADATA wsa;
	
	HWND hWnd;

	void recvEventThread();
};