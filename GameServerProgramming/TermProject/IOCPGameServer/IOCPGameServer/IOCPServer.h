#pragma once
#include "stdafx.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}


constexpr int MAX_PACKET_SIZE{ 255 };
constexpr int MAX_BUFFER_SIZE{ 1024 };
constexpr int MAX_USER_SIZE{ 10000 };
//constexpr int MAX_NPC_SIZE{ 20 };

constexpr int SECTOR_SIZE{ 10 };
constexpr int VIEW_RADIUS{ 10 };

constexpr float MOVE_TIME{ 1000.0f };
constexpr float SPAWN_TIME{ 5000.0f };

enum class EOperation : int {
	E_RECV,
	E_SEND,
	E_ACCEPT,
	E_NPCMOVE,
	E_PLAYERMOVE,
	E_RESPAWNMONSTER,
	E_ATTACKMONSTER
};

enum class EStatus : int {
	E_FREE,
	E_ALLOC,
	E_ACTIVE
};

enum class EMonsterType : int {
	E_PEACE = 1,
	E_WAR = 2,
	E_ATTACKING = 3
};
enum class EMonsterMoveType : int {
	E_FIX = 1,
	E_MOVE = 2
};

struct ExOverlapped {
	WSAOVERLAPPED overlapped{};
	EOperation operation{};
	char ioBuffer[MAX_BUFFER_SIZE]{};
	union {
		WSABUF wsabuf;
		SOCKET clientSock;
		int	   Id;
	};
};

struct Client {
	std::mutex mtx{};
	SOCKET socket{};
	int id{};
	ExOverlapped recvOverlapped{};
	int prevSize{};
	char packetBuffer[MAX_PACKET_SIZE]{};
	std::atomic<EStatus> status{};
	std::unordered_set<int> viewList{};

	short x{}, y{};
	int curSectX{}, curSectY{};
	char name[MAX_ID_LEN + 1]{};
	unsigned int moveTime{};
	std::chrono::high_resolution_clock::time_point lastAttackTime{};
	int hp{};
	int level{};
	int exp{};
	std::string systemString{};
};

struct EventType {
	int currentId{};
	int targetId{};
	std::chrono::high_resolution_clock::time_point eventTime{};

	inline bool operator<(const EventType& rhs) const {
		return eventTime > rhs.eventTime;
	}
};

struct Npc {
	int x{}, y{};
	int curSectX{}, curSectY{};
	int id{};
	int hp{};
	int level{};
	
	int oldX{}, oldY{};
	int oldSecX{}, oldSecY{};

	EMonsterType type{};
	EMonsterMoveType moveType{};
	EMonsterType oldType{};
	char name[MAX_ID_LEN + 1];
	std::mutex mtx{};
	std::unordered_set<int> viewList{};
	std::atomic<bool> isActive{};
	lua_State* L{};
	std::mutex luaMtx{};

	std::string expString{};
};