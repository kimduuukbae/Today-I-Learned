#include "stdafx.h"
#include "ServerDevice.h"
#include <WS2tcpip.h>
#include <thread>
#include "PieceManager.h"

#pragma warning(disable:4996)

bool ServerDevice::initialize(const char* ip, int port, HWND winHandle){
	hWnd = winHandle;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return false;

	if ((serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))
		== INVALID_SOCKET)
		return false;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	inet_pton(AF_INET, ip, &serverAddr.sin_addr);

	if (connect(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr))
		== SOCKET_ERROR)
		return false;

	std::thread{ &ServerDevice::recvEventThread, this }.detach();
	return true;
}

void ServerDevice::destroy(){
	closesocket(serverSocket);
	WSACleanup();
}

PlayerInfo ServerDevice::recvInfo(){
	PlayerInfo info;
	recv(serverSocket, (char*)&info, sizeof(info), 0);
	return info;
}

void ServerDevice::sendKey(const E_PACKET_TYPE& keyButton, int id){
	PlayerPackInfo info{ id, keyButton };
	int retval = send(serverSocket, (char*)&info, sizeof(PlayerPackInfo), 0);
	if (retval == 0 || retval == SOCKET_ERROR)
		closesocket(serverSocket);
}

void ServerDevice::recvEventThread(){
	while (1) {
		PlayerInfo info;
		int retval = recv(serverSocket, (char*)&info, sizeof(info), 0);
		if (retval == 0 || retval == SOCKET_ERROR)
			break;

		if (PieceManager::getInstance().find(info.PlayerID))
			PieceManager::getInstance().movePiece(
				info.PlayerID, info.pos.xPos, info.pos.yPos);
		else {
			PieceManager::getInstance().initPiece(
				E_CHESS_TYPE::E_PAWN, info.PlayerID, info.pos.xPos, info.pos.yPos);
			//sendKey(E_PACKET_TYPE::E_SPAWN, PieceManager::getInstance().getHeroID());
		}

		InvalidateRect(hWnd, NULL, true);
	}
}



