#pragma once
enum class E_CHESS_TYPE : int {
	E_NONE,
	E_PAWN,
	E_KNIGHT,
	E_VISHOP,
	E_ROOK,
	E_QUEEN,
	E_KING
};

enum class E_PACKET_TYPE : int {
	E_LEFT,
	E_RIGHT,
	E_UP,
	E_DOWN,
	E_SPAWN,
	E_SEND
};

struct Position {
	int xPos;
	int yPos;
};

struct PlayerInfo{
	int playerID;
	Position pos;
};

struct PlayerPackInfo {
	int playerID;
	E_PACKET_TYPE packetType;
};

struct SOCKETINFO {
	WSAOVERLAPPED overlapped;
	WSABUF dataBuffer;
	SOCKET socket;
	char buffers[MAXBUFFER];
};

struct PlayerSocketInfo {
	PlayerInfo pInfo;
	SOCKETINFO sInfo;
};

