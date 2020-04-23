#include <iostream>
#include <WS2tcpip.h>
#include <MSWSock.h>
#include "protocol.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

constexpr int MAX_PACKET_SIZE{ 255 };
constexpr int MAX_BUFFER_SIZE{ 1024 };
constexpr int MAX_USER_SIZE{ 10 };
enum class EOperation : int {
	E_RECV,
	E_SEND,
	E_ACCEPT
};

struct ExOverlapped {
	WSAOVERLAPPED overlapped;
	EOperation operation;
	char ioBuffer[MAX_BUFFER_SIZE];
	WSABUF wsabuf;
};

struct Client {
	SOCKET socket;
	int id;
	ExOverlapped recvOverlapped;
	int prevSize;
	char packetBuffer[MAX_PACKET_SIZE];
	bool isConnected;

	short x, y;
	char name[MAX_ID_LEN + 1];
};

Client clients[MAX_USER_SIZE]{};
int currentUserId{ 0 };
HANDLE iocpHandle;

void sendPacket(int userId, void* packet) {
	char* buf{ reinterpret_cast<char*>(packet) };
	Client& user{ clients[userId] };

	ExOverlapped* exOver = new ExOverlapped{};
	ZeroMemory(&exOver->overlapped, sizeof(exOver->overlapped));
	exOver->operation = EOperation::E_SEND;
	exOver->wsabuf.buf = exOver->ioBuffer;
	exOver->wsabuf.len = buf[0];
	memcpy(exOver->ioBuffer, buf, buf[0]);

	WSASend(user.socket, &exOver->wsabuf, 1, NULL, 0, &exOver->overlapped, NULL);
}

void sendLoginPacket(int userId) {
	sc_packet_login_ok packet{};
	packet.exp = 0;
	packet.hp = 0;
	packet.id = userId;
	packet.level = 0;
	packet.size = sizeof(packet);
	packet.type = S2C_LOGIN_OK;
	packet.x = clients[userId].x;
	packet.y = clients[userId].y;

	sendPacket(userId, &packet);
}

void sendMovePacket(int userId, int targetId) {
	sc_packet_move packet{};
	packet.id = targetId;
	packet.size = sizeof(packet);
	packet.type = S2C_MOVE;
	packet.x = clients[targetId].x;
	packet.y = clients[targetId].y;

	sendPacket(userId, &packet);
}

void sendEnterPacket(int userId, int targetId) {
	sc_packet_enter packet{};
	packet.id = targetId;
	packet.size = sizeof(packet);
	packet.type = S2C_ENTER;
	packet.x = clients[targetId].x;
	packet.y = clients[targetId].y;
	strcpy_s(packet.name, clients[targetId].name);
	packet.o_type = O_PLAYER;

	sendPacket(userId, &packet);
}

void sendLeavePacket(int userId, int targetId) {
	sc_packet_leave packet{};
	packet.id = targetId;
	packet.size = sizeof(packet);
	packet.type = S2C_LEAVE;

	sendPacket(userId, &packet);
}

void clientMove(int userId, int direction) {
	Client& user{ clients[userId] };
	int x{ user.x };
	int y{ user.y };

	switch (direction) {
	case D_UP:
		if (y > 0) --y;
		break;
	case D_DOWN:
		if (y < WORLD_HEIGHT - 1) ++y;
		break;
	case D_LEFT:
		if (x > 0) --x;
		break;
	case D_RIGHT:
		if (x < WORLD_WIDTH - 1) ++x;
		break;
	default:
		DebugBreak();
		exit(-1);
	}
	user.x = x;
	user.y = y;
	for (auto& client : clients) {
		if (client.isConnected)
			sendMovePacket(client.id, userId);
	}
}

void enterGame(int userId) {
	clients[userId].isConnected = true;
	for (int i = 0; i < MAX_USER_SIZE; ++i) {
		if (clients[i].isConnected && !(userId & i)) {
			sendEnterPacket(userId, i);
			sendEnterPacket(i, userId);
		}
	}

}

void clientInit() {
	for (int i = 0; i < MAX_USER_SIZE; ++i)
		clients[i].isConnected = false;
}

void disconnect(int userId) {
	clients[userId].isConnected = false;
	for (auto& client : clients) {
		if (client.isConnected)
			sendLeavePacket(client.id, userId);
	}
}

void processPacket(int userId, char* buf) {
	switch (buf[1]) {
	case C2S_LOGIN: {
		cs_packet_login* packet{ reinterpret_cast<cs_packet_login*>(buf) };
		strcpy_s(clients[userId].name, packet->name);
		clients[userId].name[MAX_ID_LEN] = '\0';
		sendLoginPacket(userId);
		enterGame(userId);
		break;
	}
	case C2S_MOVE: {
		cs_packet_move* packet = reinterpret_cast<cs_packet_move*>(buf);
		clientMove(userId, packet->direction);
		break;
	}
	default:
		DebugBreak();
		exit(-1);
	}
}

void recvPacketConstruct(int userId, int ioByte) {
	Client& user{ clients[userId] };
	ExOverlapped& recvOver{ user.recvOverlapped };
	
	int restByte{ ioByte };
	char* currentBuffer{ recvOver.ioBuffer };
	int packetSize{ 0 };

	if (user.prevSize != 0)
		packetSize = user.packetBuffer[0];

	while (restByte > 0) {
		if (packetSize == 0)
			packetSize = currentBuffer[0];
		if (packetSize <= restByte + user.prevSize) {
			memcpy(user.packetBuffer + user.prevSize, currentBuffer, packetSize - user.prevSize);
			currentBuffer += packetSize - user.prevSize;
			restByte -= packetSize - user.prevSize;
			packetSize = 0;
			user.prevSize = 0;
			processPacket(userId, user.packetBuffer);
		}
		else {
			memcpy(user.packetBuffer + user.prevSize, currentBuffer, restByte);
			user.prevSize += restByte;
			restByte = 0;
			currentBuffer += restByte;
		}
	}
}

int main() {
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET listenSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP,
		NULL, 0, WSA_FLAG_OVERLAPPED);

	sockaddr_in serverAddr{};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	bind(listenSock, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));

	listen(listenSock, SOMAXCONN);

	iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);

	clientInit();

	CreateIoCompletionPort(reinterpret_cast<HANDLE>(listenSock), iocpHandle, -1, 0);
	SOCKET clientSock{ WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED) };
	ExOverlapped acceptOver{};
	acceptOver.operation = EOperation::E_ACCEPT;
	AcceptEx(listenSock, clientSock, acceptOver.ioBuffer, NULL, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,
		NULL, &acceptOver.overlapped);

	while (true) {
		DWORD ioByte{};
		ULONG_PTR key{};
		WSAOVERLAPPED* over;
		GetQueuedCompletionStatus(iocpHandle, &ioByte, &key, &over, INFINITE);

		ExOverlapped* exOver{ reinterpret_cast<ExOverlapped*>(over) };
		int userId{ static_cast<int>(key) };
		Client& curClient{ clients[userId] };

		switch (exOver->operation) {
		case EOperation::E_RECV: {
			if (ioByte == 0) 
				disconnect(userId);
			else {
				recvPacketConstruct(userId, ioByte);
				DWORD flags{ 0 };
				ZeroMemory(&curClient.recvOverlapped.overlapped,
					sizeof(curClient.recvOverlapped.overlapped));

				WSARecv(curClient.socket, &curClient.recvOverlapped.wsabuf, 1, NULL,
					&flags, &curClient.recvOverlapped.overlapped, NULL);
			}
			break;
		}
		case EOperation::E_SEND:
			if (ioByte == 0)
				disconnect(userId);
			delete exOver;
			break;
		case EOperation::E_ACCEPT: {
			int userId{ currentUserId++ };
			currentUserId = currentUserId % MAX_USER_SIZE;
			Client& targetClient{ clients[userId] };
			CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSock), iocpHandle, userId, 0);

			targetClient.id = userId;
			targetClient.prevSize = 0;
			targetClient.recvOverlapped.operation = EOperation::E_RECV;
			targetClient.recvOverlapped.wsabuf.buf = targetClient.recvOverlapped.ioBuffer;
			targetClient.recvOverlapped.wsabuf.len = MAX_BUFFER_SIZE;
			targetClient.socket = clientSock;
			targetClient.x = rand() % WORLD_WIDTH;
			targetClient.y = rand() % WORLD_HEIGHT;

			DWORD flags{ 0 };

			WSARecv(clientSock, &targetClient.recvOverlapped.wsabuf, 1, NULL,
				&flags, &targetClient.recvOverlapped.overlapped, NULL);

			clientSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
			ZeroMemory(&acceptOver.overlapped, sizeof(acceptOver.overlapped));
			AcceptEx(listenSock, clientSock, acceptOver.ioBuffer, NULL, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,
				NULL, &acceptOver.overlapped);
			break;
		}
		}
	}
}