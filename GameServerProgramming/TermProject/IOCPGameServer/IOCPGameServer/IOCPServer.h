#pragma once

#include <WS2tcpip.h>
#include <MSWSock.h>
#include <mutex>
#include <atomic>
#include <chrono>
#include <unordered_set>

#include "protocol.h"

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


enum class EOperation : int {
	E_RECV,
	E_SEND,
	E_ACCEPT,
	E_NPCMOVE,
	E_PLAYERMOVE
};

enum class EStatus : int {
	E_FREE,
	E_ALLOC,
	E_ACTIVE
};

struct ExOverlapped {
	WSAOVERLAPPED overlapped{};
	EOperation operation{};
	char ioBuffer[MAX_BUFFER_SIZE]{};
	union {
		WSABUF wsabuf;
		SOCKET clientSock;
		int	   playerId;
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

	char name[MAX_ID_LEN + 1];
	std::mutex mtx{};
	std::unordered_set<int> viewList{};
	std::atomic<bool> isActive{};
	lua_State* L{};
	std::mutex luaMtx{};
	std::atomic<int> moveCount{};
};