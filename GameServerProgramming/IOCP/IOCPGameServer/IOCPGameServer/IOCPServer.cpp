#include <iostream>
#include <thread>
#include <queue>
#include <tuple>
#include <string>
#include <vector>
#include <array>
#include <codecvt>
#include <concurrent_priority_queue.h>
#include "IOCPServer.h"
#include "Database.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")
#pragma comment(lib, "lua53.lib")

Client clients[MAX_USER_SIZE]{};
HANDLE iocpHandle{};
SOCKET listenSock{};
Database dbDevice{};
Npc npcs[MAX_NPC_SIZE]{};
concurrency::concurrent_priority_queue <EventType> eventQueue{};
std::array<std::array<std::unordered_set<int>, WORLD_HEIGHT / SECTOR_SIZE>, WORLD_WIDTH / SECTOR_SIZE> npcSector{};
std::array<std::array<std::unordered_set<int>, WORLD_HEIGHT / SECTOR_SIZE>, WORLD_WIDTH / SECTOR_SIZE> clientsSector{};
std::mutex clientsSectorMtx[WORLD_WIDTH / SECTOR_SIZE][WORLD_HEIGHT / SECTOR_SIZE]{};
std::mutex npcSectorMtx[WORLD_WIDTH / SECTOR_SIZE][WORLD_HEIGHT / SECTOR_SIZE]{};

std::wstring utf8_to_wstring(const std::string& str) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	return myconv.from_bytes(str);
}

void saveAll() {
	for (int i = 0; i < MAX_USER_SIZE; ++i) {
		if (clients[i].status == EStatus::E_ACTIVE)
			dbDevice.setUserInfo(std::make_tuple(utf8_to_wstring(clients[i].name), clients[i].x, clients[i].y));
	}
}

std::pair<int, int> findPath(const Npc& target) {
	int x{ target.x }, y{ target.y };
	int direction{ rand() % 4 };
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
	return { x, y };
}

std::pair<int, int> getSectorPos(int xPos, int yPos) {
	return { xPos / SECTOR_SIZE, yPos / SECTOR_SIZE };
}

bool isInArea(int userId, int targetId) {
	return (std::abs(clients[userId].x - clients[targetId].x) < VIEW_RADIUS) &
		(std::abs(clients[userId].y - clients[targetId].y) < VIEW_RADIUS);
}

bool isInArea(int userX, int userY, int targetX, int targetY) {
	return (std::abs(userX - targetX) < VIEW_RADIUS) &
		(std::abs(userY - targetY) < VIEW_RADIUS);
}

void sendPacket(int userId, void* packet) {
	char* buf{ reinterpret_cast<char*>(packet) };
	ExOverlapped* exOver{ new ExOverlapped{} };

	exOver->operation = EOperation::E_SEND;
	exOver->wsabuf.buf = exOver->ioBuffer;
	exOver->wsabuf.len = buf[0];
	memcpy(exOver->ioBuffer, buf, buf[0]);

	WSASend(clients[userId].socket, &exOver->wsabuf, 1, NULL, 0, &exOver->overlapped, NULL);
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
	packet.move_time = clients[targetId].moveTime;
	sendPacket(userId, &packet);
}

void sendMovePacketNpc(int userId, int npcId) {
	sc_packet_move packet{};
	packet.id = npcId;
	packet.size = sizeof(packet);
	packet.type = S2C_MOVE;
	packet.x = npcs[npcId - 10001].x;
	packet.y = npcs[npcId - 10001].y;
	packet.move_time = 0;
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

void sendEnterPacketNpc(int userId, int npcId) {
	sc_packet_enter packet{};
	packet.id = npcId;
	packet.size = sizeof(packet);
	packet.type = S2C_ENTER;
	packet.x = npcs[npcId - 10001].x;
	packet.y = npcs[npcId - 10001].y;
	strcpy_s(packet.name, "NPC");
	packet.o_type = O_NPC;

	sendPacket(userId, &packet);
}

void sendChatPacket(int userId, int chatId, const char* message) {
	sc_packet_chat packet{};
	packet.id = chatId;
	packet.size = sizeof(packet);
	packet.type = S2C_CHAT;
	strcpy_s(packet.message, message);

	sendPacket(userId, &packet);
}

void sendLeavePacket(int userId, int targetId) {
	sc_packet_leave packet{};
	packet.id = targetId;
	packet.size = sizeof(packet);
	packet.type = S2C_LEAVE;

	sendPacket(userId, &packet);
}

bool searchSectorNpc(int npcId, int secPosX, int secPosY) {

	if (secPosX < 0 || secPosX >(WORLD_WIDTH / SECTOR_SIZE) - 1 ||
		secPosY < 0 || secPosY >(WORLD_WIDTH / SECTOR_SIZE) - 1)
		return false;

	bool bFlags{ false };

	std::vector<int> v{};

	clientsSectorMtx[secPosX][secPosY].lock();
	v.reserve(clientsSector[secPosX][secPosY].size());
	for(auto& i : clientsSector[secPosX][secPosY])
		v.emplace_back(i);
	clientsSectorMtx[secPosX][secPosY].unlock();

	for (auto idCluster : v) {
		if (isInArea(clients[idCluster].x, clients[idCluster].y, npcs[npcId].x, npcs[npcId].y)) {
			npcs[npcId].mtx.lock();
			if (npcs[npcId].viewList.count(idCluster)) {
				npcs[npcId].mtx.unlock();
				bFlags = true;
				sendMovePacketNpc(idCluster, npcId + 10001);
			}
			else {
				npcs[npcId].viewList.emplace(idCluster);
				npcs[npcId].mtx.unlock();
				sendEnterPacketNpc(idCluster, npcId + 10001);
				bFlags = true;
			}
				
		}
		else {
			npcs[npcId].mtx.lock();
			if (npcs[npcId].viewList.count(idCluster)) {
				npcs[npcId].viewList.erase(idCluster);
				npcs[npcId].mtx.unlock();
				sendLeavePacket(idCluster, npcId + 10001);
			}
			else
				npcs[npcId].mtx.unlock();
		}
	}
	return bFlags;
}

void searchSector(int userId, int secPosX, int secPosY, std::unordered_set<int>& userViewList) {

	if (secPosX < 0 || secPosX > (WORLD_WIDTH / SECTOR_SIZE) - 1 ||
		secPosY < 0 || secPosY > (WORLD_WIDTH / SECTOR_SIZE) - 1)
		return;

	Client& user{ clients[userId] };
	clientsSectorMtx[secPosX][secPosY].lock();
	std::unordered_set<int> oldSector{ clientsSector[secPosX][secPosY] };
	clientsSectorMtx[secPosX][secPosY].unlock();

	std::vector<int> v{};

	npcSectorMtx[secPosX][secPosY].lock();
	v.reserve(npcSector[secPosX][secPosY].size());
	for (auto& i : npcSector[secPosX][secPosY])
		v.emplace_back(i);
	npcSectorMtx[secPosX][secPosY].unlock();

	for (auto idCluster : v) {
		int ClusterIdx{ idCluster - 10001 };
		if (isInArea(user.x, user.y, npcs[ClusterIdx].x, npcs[ClusterIdx].y)) {
			if (!npcs[ClusterIdx].isActive && user.x == npcs[ClusterIdx].x &&
				user.y == npcs[ClusterIdx].y) {
				npcs[ClusterIdx].isActive = true;
				npcs[ClusterIdx].moveCount = 3;
				eventQueue.push(EventType{ idCluster, userId, std::chrono::high_resolution_clock::now() });
			}
			npcs[ClusterIdx].mtx.lock();
			if (!npcs[ClusterIdx].viewList.count(userId)) {
				npcs[ClusterIdx].viewList.emplace(userId);
				npcs[ClusterIdx].mtx.unlock();
				sendEnterPacketNpc(userId, idCluster);
			}
			else
				npcs[ClusterIdx].mtx.unlock();

			ExOverlapped* exOver{ new ExOverlapped{} };
			exOver->operation = EOperation::E_PLAYERMOVE;
			exOver->playerId = userId;
			PostQueuedCompletionStatus(iocpHandle, 1, idCluster, &exOver->overlapped);
		}
		else {
			npcs[ClusterIdx].mtx.lock();
			if (npcs[ClusterIdx].viewList.count(userId)) {
				npcs[ClusterIdx].viewList.erase(userId);
				npcs[ClusterIdx].mtx.unlock();
				sendLeavePacket(userId, idCluster);
			}
			else
				npcs[ClusterIdx].mtx.unlock();
		}
	}
	for (auto idCluster : oldSector) {
		if (userViewList.count(idCluster)) {
			// 뷰 리스트에 있고 시야에도 보임
			if (isInArea(userId, idCluster))
				sendMovePacket(idCluster, userId);
			else {
				// 뷰 리스트에 있는데, 시야에 보이질 않음
				clients[idCluster].mtx.lock();
				if (clients[idCluster].viewList.count(userId)) {
					clients[idCluster].viewList.erase(userId);
					clients[idCluster].mtx.unlock();
					sendLeavePacket(idCluster, userId);
				}
				else
					clients[idCluster].mtx.unlock();

				userViewList.erase(idCluster);
				sendLeavePacket(userId, idCluster);
			}
		}
		else {
			if (isInArea(userId, idCluster)) {
				// 뷰 리스트에 없는데, 시야에 보임
				clients[idCluster].mtx.lock();
				if (!clients[idCluster].viewList.count(userId)) {
					clients[idCluster].viewList.emplace(userId);
					clients[idCluster].mtx.unlock();
					sendEnterPacket(idCluster, userId);
				}
				else
					clients[idCluster].mtx.unlock();
				userViewList.emplace(idCluster);
				sendEnterPacket(userId, idCluster);
			}
		}
	}
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

	auto [secX, secY] {getSectorPos(user.x, user.y)};

	clients[userId].mtx.lock();
	std::unordered_set<int> oldViewList{ clients[userId].viewList };
	clients[userId].mtx.unlock();

	for (int i = -1; i < 2; ++i)
		for (int j = -1; j < 2; ++j)
			searchSector(userId, user.curSectX + i, user.curSectY + j, oldViewList);

	if (user.curSectX != secX || user.curSectY != secY) {

		clientsSectorMtx[user.curSectX][user.curSectY].lock();
		clientsSector[user.curSectX][user.curSectY].erase(user.id);
		clientsSectorMtx[user.curSectX][user.curSectY].unlock();

		clientsSectorMtx[secX][secY].lock();
		clientsSector[secX][secY].emplace(user.id);
		clientsSectorMtx[secX][secY].unlock();

		user.curSectX = secX;
		user.curSectY = secY;
	}

	clients[userId].mtx.lock();
	clients[userId].viewList.swap(oldViewList);
	clients[userId].mtx.unlock();
}

void enterGame(int userId, char name[]) {
	Client& user{ clients[userId] };
	
	auto [id, x, y] {dbDevice.getUserInfo(utf8_to_wstring(name))};
	if (id == L"ERROR") {
		closesocket(user.socket);
		user.status = EStatus::E_FREE;
	}
	else {
		user.x = x;
		user.y = y;
		user.mtx.lock();
		strcpy_s(user.name, name);
		user.name[MAX_ID_LEN] = '\0';
		std::unordered_set<int> oldViewList{};
		user.mtx.unlock();

		sendLoginPacket(userId);
		user.status = EStatus::E_ACTIVE;

		auto [secX, secY] {getSectorPos(user.x, user.y)};

		clientsSectorMtx[secX][secY].lock();
		clientsSector[secX][secY].emplace(userId);
		clientsSectorMtx[secX][secY].unlock();
		user.curSectX = secX;
		user.curSectY = secY;

		searchSector(userId, user.curSectX, user.curSectY, oldViewList);
		searchSector(userId, user.curSectX - 1, user.curSectY, oldViewList);
		searchSector(userId, user.curSectX + 1, user.curSectY, oldViewList);
		searchSector(userId, user.curSectX, user.curSectY - 1, oldViewList);
		searchSector(userId, user.curSectX, user.curSectY + 1, oldViewList);
	}
}

int API_send_message(lua_State* L) {
	int playerId{ static_cast<int>(lua_tointeger(L, -3)) };
	int objId{ static_cast<int>(lua_tointeger(L, -2)) };
	char* message{ (char*)lua_tostring(L, -1)};

	lua_pop(L, 3);
	sendChatPacket(playerId, objId + 10001, message);
	return 0;
}

int API_get_x(lua_State* L) {
	int obj_id = (int)lua_tointeger(L, -1);
	int x{ clients[obj_id].x };
	lua_pushnumber(L, x);
	return 1;
}

int API_get_y(lua_State* L) {
	int obj_id = (int)lua_tointeger(L, -1);
	int y{ clients[obj_id].y };
	lua_pushnumber(L, y);
	return 1;
}

int API_get_x_NPC(lua_State* L) {
	int obj_id = (int)lua_tointeger(L, -1);
	int x{ npcs[obj_id].x };
	lua_pushnumber(L, x);
	return 1;
}

int API_get_y_NPC(lua_State* L) {
	int obj_id = (int)lua_tointeger(L, -1);
	int y{ npcs[obj_id].y };
	lua_pushnumber(L, y);
	return 1;
}

void clientInit() {

	for (auto& it : npcSector) {
		for (auto& idCluster : it)
			idCluster.reserve(SECTOR_SIZE * SECTOR_SIZE * 2);
	}

	for (auto& it : clientsSector) {
		for (auto& idCluster : it)
			idCluster.reserve(SECTOR_SIZE * 2);
	}

	for (int i = 0; i < MAX_USER_SIZE; ++i) {
		clients[i].id = i;
		clients[i].viewList.reserve(100);
		clients[i].viewList.emplace(i);
	}
	for (int i = 0; i < MAX_NPC_SIZE; ++i) {
		npcs[i].x = rand() % WORLD_WIDTH;
		npcs[i].y = rand() % WORLD_HEIGHT;
		npcs[i].id = i + 10001;

		auto [secX, secY] {getSectorPos(npcs[i].x, npcs[i].y)};
		npcSector[secX][secY].emplace(npcs[i].id);
		npcs[i].curSectX = secX;
		npcs[i].curSectY = secY;
		npcs[i].moveCount = 3;
		lua_State* L = npcs[i].L = luaL_newstate();
		luaL_openlibs(L);
		luaL_loadfile(L, "NPC.LUA");
		lua_pcall(L, 0, 0, 0);
		lua_getglobal(L, "setId");
		lua_pushnumber(L, i);
		lua_pcall(L, 1, 0, 0);
		lua_pop(L, 1);

		lua_register(L, "API_send_message", API_send_message);
		lua_register(L, "API_get_x", API_get_x);
		lua_register(L, "API_get_y", API_get_y);
		lua_register(L, "API_get_x_NPC", API_get_x_NPC);
		lua_register(L, "API_get_y_NPC", API_get_y_NPC);
	}

	std::cout << "end Init" << std::endl;
}

void disconnect(int userId) {
	int secX{ clients[userId].curSectX };
	int secY{ clients[userId].curSectY };
	Client& user{ clients[userId] };

	clientsSectorMtx[secX][secY].lock();
	clientsSector[secX][secY].erase(userId);
	clientsSectorMtx[secX][secY].unlock();
	user.status = EStatus::E_ALLOC;
	sendLeavePacket(userId, userId);
	dbDevice.setUserInfo(std::make_tuple(utf8_to_wstring(user.name), user.x, user.y));

	closesocket(user.socket);
	for (auto& client : clients) {
		if (userId == client.id)
			continue;

		if (client.status == EStatus::E_ACTIVE)
			sendLeavePacket(client.id, userId);
	}
	user.status = EStatus::E_FREE;
}

void processPacket(int userId, char* buf) {
	switch (buf[1]) {
	case C2S_LOGIN: {
		cs_packet_login* packet{ reinterpret_cast<cs_packet_login*>(buf) };
		enterGame(userId, packet->name);
		break;
	}
	case C2S_MOVE: {
		cs_packet_move* packet = reinterpret_cast<cs_packet_move*>(buf);
		clients[userId].moveTime = packet->move_time;
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

void timerThread() {
	while (1) {
		EventType tmp{};
		if (eventQueue.try_pop(tmp)) {
			auto curTime{ std::chrono::high_resolution_clock::now() };

			if (std::chrono::duration<float, std::milli>
				(curTime - tmp.eventTime).count() > 1000.0f) {
				ExOverlapped* exOver{ new ExOverlapped{} };
				exOver->operation = EOperation::E_NPCMOVE;
				exOver->playerId = tmp.targetId;
				PostQueuedCompletionStatus(iocpHandle, 1, tmp.currentId, &exOver->overlapped);
			}
			else {
				eventQueue.push(std::move(tmp));
				Sleep(1);
			}
		}
		else
			Sleep(1);
	}
}

void workerThread() {
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
			SOCKET clientSock = exOver->clientSock;
			int userId{ -1 };
			for (int i = 0; i < MAX_USER_SIZE; ++i) {
				if (clients[i].status == EStatus::E_FREE) {
					userId = i;
					clients[i].status = EStatus::E_ALLOC;
					break;
				}
			}
			if (userId == -1)
				closesocket(exOver->clientSock);
			else {
				Client& targetClient{ clients[userId] };
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(clientSock), iocpHandle, userId, 0);

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
			}
			clientSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
			exOver->clientSock = clientSock;
			ZeroMemory(&exOver->overlapped, sizeof(exOver->overlapped));
			AcceptEx(listenSock, clientSock, exOver->ioBuffer, NULL, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,
				NULL, &exOver->overlapped);
			break;
		}
		
		case EOperation::E_NPCMOVE: {
			key = key - 10001;
			const auto [x, y] {findPath(npcs[key])};
			Npc& npc{ npcs[key] };

			npc.x = x;
			npc.y = y;

			auto [secX, secY] {getSectorPos(x, y)};

			for (int i = -1; i < 2; ++i)
				for (int j = -1; j < 2; ++j)
					searchSectorNpc(key, npc.curSectX + i, npc.curSectY + j);

			if (npc.curSectX != secX || npc.curSectY != secY) {
				npcSectorMtx[npc.curSectX][npc.curSectY].lock();
				npcSector[npc.curSectX][npc.curSectY].erase(npc.id);
				npcSectorMtx[npc.curSectX][npc.curSectY].unlock();

				npcSectorMtx[secX][secY].lock();
				npcSector[secX][secY].emplace(npc.id);
				npcSectorMtx[secX][secY].unlock();


				npc.curSectX = secX;
				npc.curSectY = secY;
			}

			if (npcs[key].moveCount > 1) {
				--npcs[key].moveCount;
				eventQueue.push(EventType{ static_cast<int>(key + 10001), exOver->playerId, std::chrono::high_resolution_clock::now() });

			}
			else {
				npcs[key].isActive = false;
				sendChatPacket(exOver->playerId, key + 10001, "BYE");
			}
			delete exOver;
			break;
		}
		
		case EOperation::E_PLAYERMOVE: {
			npcs[key-10001].luaMtx.lock();
			lua_State*& L{npcs[key - 10001].L};
			lua_getglobal(L, "eventPlayerMove");
			lua_pushnumber(L, exOver->playerId);

			if (int error{ lua_pcall(L, 1, 0, 0) }; error) {
				std::cout << lua_tostring(L, -1);
				lua_pop(L, 1);
			}

			npcs[key - 10001].luaMtx.unlock();
			delete exOver;
			break;
		}
		}
	}
}

int main() {
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	listenSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP,
		NULL, 0, WSA_FLAG_OVERLAPPED);

	sockaddr_in serverAddr{};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	bind(listenSock, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));

	listen(listenSock, SOMAXCONN);

	iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);

	clientInit();

	CreateIoCompletionPort(reinterpret_cast<HANDLE>(listenSock), iocpHandle, 999, 0);
	SOCKET clientSock{ WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED) };
	ExOverlapped acceptOver{};
	acceptOver.operation = EOperation::E_ACCEPT;
	acceptOver.clientSock = clientSock;
	AcceptEx(listenSock, clientSock, acceptOver.ioBuffer, NULL, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,
		NULL, &acceptOver.overlapped);

	std::vector<std::thread> workers{};
	dbDevice.initalize(L"2020GS_2014180025");
	atexit(saveAll);

	for (int i = 0; i < 4; ++i)
		workers.emplace_back(workerThread);
	workers.emplace_back(timerThread);
	for (auto& th : workers)
		th.join();


}