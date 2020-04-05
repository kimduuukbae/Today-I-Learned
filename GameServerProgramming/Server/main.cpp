#include "stdafx.h"
#include "Board.h"
#include <map>

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

#define D_MAPXSIZE 8
#define D_MAPYSIZE 8

int nextID = { 1 };
Board board;
std::map<SOCKET, PlayerSocketInfo> clientSockets;
char chessBuffer[MAXBUFFER] = {};

void CALLBACK sendCallback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD InFlags);
void CALLBACK recvCallback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD InFlags);

void CALLBACK recvCallback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD InFlags) {

	SOCKET sock = reinterpret_cast<int>(overlapped->hEvent);
	if (dataBytes == 0) {
		closesocket(clientSockets[sock].sInfo.socket);
		clientSockets.erase(sock);
		return;
	}
	else if (Error == SOCKET_ERROR)
		return;

	int heroX = clientSockets[sock].pInfo.pos.xPos;
	int heroY = clientSockets[sock].pInfo.pos.yPos;

	PlayerPackInfo packInfo = {};
	::memcpy((char*)&packInfo, 
		clientSockets[sock].sInfo.dataBuffer.buf,
		clientSockets[sock].sInfo.dataBuffer.len);

	
	switch (packInfo.packetType) {
	case E_PACKET_TYPE::E_LEFT:
		heroX = heroX > 0 ? heroX - 1 : 0;
		break;
	case E_PACKET_TYPE::E_RIGHT:
		heroX = heroX < D_MAPXSIZE - 1 ? heroX + 1 : D_MAPXSIZE - 1;
		break;
	case E_PACKET_TYPE::E_UP:
		heroY = heroY > 0 ? heroY - 1 : 0;
		break;
	case E_PACKET_TYPE::E_DOWN:
		heroY = heroY < D_MAPYSIZE - 1 ? heroY + 1 : D_MAPYSIZE - 1;
		break;
	}

	clientSockets[sock].pInfo.pos.xPos = heroX;
	clientSockets[sock].pInfo.pos.yPos = heroY;

	::memcpy(clientSockets[sock].sInfo.dataBuffer.buf,
	(char*)&clientSockets[sock].pInfo,
	sizeof(clientSockets[sock].pInfo));

	for (auto& i : clientSockets) {
		memset(&(i.second.sInfo.overlapped), 0, sizeof(WSAOVERLAPPED));
		i.second.sInfo.overlapped.hEvent = (HANDLE)i.first;
		i.second.sInfo.dataBuffer.len = sizeof(clientSockets[sock].pInfo);
		::memcpy(clientSockets[i.first].sInfo.dataBuffer.buf,
			(char*)&clientSockets[sock].pInfo,
			sizeof(clientSockets[sock].pInfo));
		WSASend(i.first, &(clientSockets[i.first].sInfo.dataBuffer), 1, NULL,
		0, &(clientSockets[i.first].sInfo.overlapped), sendCallback);
	}
}

void CALLBACK sendCallback(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD InFlags) {

	SOCKET sock = reinterpret_cast<int>(overlapped->hEvent);
	if (dataBytes == 0) {
		closesocket(clientSockets[sock].sInfo.socket);
		clientSockets.erase(sock);
		return;
	}
	DWORD flags = { 0 };

	clientSockets[sock].sInfo.dataBuffer.len =
		sizeof(PlayerPackInfo);
	
	memset(&(clientSockets[sock].sInfo.overlapped), 0, sizeof(WSAOVERLAPPED));
	clientSockets[sock].sInfo.overlapped.hEvent = (HANDLE)sock;
	WSARecv(sock, &clientSockets[sock].sInfo.dataBuffer, 1, NULL,
		&flags, &(clientSockets[sock].sInfo.overlapped), recvCallback);
}
void CALLBACK sendOnce(DWORD Error, DWORD dataBytes, LPWSAOVERLAPPED overlapped, DWORD InFlags) {
	SOCKET sock = reinterpret_cast<int>(overlapped->hEvent);
	if (dataBytes == 0) {
		closesocket(clientSockets[sock].sInfo.socket);
		clientSockets.erase(sock);
		return;
	}

	memset(&(clientSockets[sock].sInfo.overlapped), 0, sizeof(WSAOVERLAPPED));
	clientSockets[sock].sInfo.overlapped.hEvent = (HANDLE)sock;
}

int main() {
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return -1;

	SOCKET serverSocket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, 
		NULL, 0, WSA_FLAG_OVERLAPPED);
	if (serverSocket == INVALID_SOCKET) std::cout << " Error " << std::endl;

	sockaddr_in socketAddr;
	socketAddr.sin_family = AF_INET;
	socketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	socketAddr.sin_port = htons(SERVERPORT);

	if (bind(serverSocket, (sockaddr*)&socketAddr, sizeof(socketAddr)) == SOCKET_ERROR)
		std::cout << " Error " << std::endl;

	if (listen(serverSocket, 10) == SOCKET_ERROR)
		std::cout << " Error " << std::endl;

	board.initialize(D_MAPXSIZE, D_MAPYSIZE);

	sockaddr_in clientAddr;
	int length{ sizeof(clientAddr) };

	while (1) {
		SOCKET connectedSock = accept(serverSocket, (sockaddr*)&clientAddr, &length);
		if (connectedSock == INVALID_SOCKET) std::cout << " Error " << std::endl;

		int heroX, heroY;
		do {
			heroX = rand() % 8;
			heroY = rand() % 8;
		} while (board.getActorTypeInBoard(heroX, heroY) != E_CHESS_TYPE::E_NONE);
		board.setActorInBoard(E_CHESS_TYPE::E_PAWN, heroX, heroY);

		clientSockets[connectedSock] = PlayerSocketInfo{};
		clientSockets[connectedSock].pInfo.pos.xPos = heroX;
		clientSockets[connectedSock].pInfo.pos.yPos = heroY;
		clientSockets[connectedSock].pInfo.playerID = nextID;
		clientSockets[connectedSock].sInfo.socket = connectedSock;

		clientSockets[connectedSock].sInfo.dataBuffer.len = sizeof(clientSockets[connectedSock].pInfo);
		clientSockets[connectedSock].sInfo.dataBuffer.buf = clientSockets[connectedSock].sInfo.buffers;

		::memset(&clientSockets[connectedSock].sInfo.overlapped, 0, sizeof(WSAOVERLAPPED));
		clientSockets[connectedSock].sInfo.overlapped.hEvent
			= (HANDLE)clientSockets[connectedSock].sInfo.socket;
	
		::memcpy(clientSockets[connectedSock].sInfo.dataBuffer.buf,
		(char*)&clientSockets[connectedSock].pInfo,
		sizeof(clientSockets[connectedSock].pInfo));
		WSASend(connectedSock, &(clientSockets[connectedSock].sInfo.dataBuffer), 1, NULL,
			0, &(clientSockets[connectedSock].sInfo.overlapped), sendCallback);

		if (nextID > 1) {
			::memcpy(chessBuffer,
				(char*)&clientSockets[connectedSock].pInfo,
				sizeof(clientSockets[connectedSock].pInfo));
			
			WSABUF wsaBuffer;
			wsaBuffer.buf = chessBuffer;
			wsaBuffer.len = sizeof(clientSockets[connectedSock].pInfo);

			// 기존 & 신규 애들에게 위치 보고
			for (auto& i : clientSockets) {
				if (i.first != connectedSock) {
					::memset(&clientSockets[i.first].sInfo.overlapped, 0, sizeof(WSAOVERLAPPED));

					clientSockets[i.first].sInfo.overlapped.hEvent = (HANDLE)i.first;
					WSASend(i.first, &(wsaBuffer), 1, NULL,
						0, &(clientSockets[i.first].sInfo.overlapped), sendCallback);
				}
			}
		}
		++nextID;
	}
	closesocket(serverSocket);
	WSACleanup();
	std::cout << "종료되었습니다." << std::endl;
}