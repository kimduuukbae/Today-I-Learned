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
Npc npcs[NUM_NPC]{};
concurrency::concurrent_priority_queue <EventType> eventQueue{};
std::array<std::array<std::unordered_set<int>, WORLD_HEIGHT / SECTOR_SIZE>, WORLD_WIDTH / SECTOR_SIZE> npcSector{};
std::array<std::array<std::unordered_set<int>, WORLD_HEIGHT / SECTOR_SIZE>, WORLD_WIDTH / SECTOR_SIZE> clientsSector{};
std::mutex clientsSectorMtx[WORLD_WIDTH / SECTOR_SIZE][WORLD_HEIGHT / SECTOR_SIZE]{};
std::mutex npcSectorMtx[WORLD_WIDTH / SECTOR_SIZE][WORLD_HEIGHT / SECTOR_SIZE]{};

std::wstring utf8_to_wstring(const std::string_view& str) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	return myconv.from_bytes(str.data());
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

bool isInArea(int userId, int targetId, int size) {
	return (std::abs(clients[userId].x - clients[targetId].x) < size) &
		(std::abs(clients[userId].y - clients[targetId].y) < size);
}

bool isInArea(int userX, int userY, int targetX, int targetY, int size) {
	return (std::abs(userX - targetX) < size) &
		(std::abs(userY - targetY) < size);
}

void sendPacket(int userId, void* packet) {
	char* buf{ reinterpret_cast<char*>(packet) };
	ExOverlapped* exOver{ new ExOverlapped{} };

	exOver->operation = EOperation::E_SEND;
	exOver->wsabuf.buf = exOver->ioBuffer;
	exOver->wsabuf.len = unsigned char(buf[0]);
	memcpy(exOver->ioBuffer, buf, unsigned char(buf[0]));

	WSASend(clients[userId].socket, &exOver->wsabuf, 1, NULL, 0, &exOver->overlapped, NULL);
}

void sendLoginPacket(int userId) {
	sc_packet_login_ok packet{};
	packet.exp = clients[userId].exp;
	packet.hp = clients[userId].hp;
	packet.id = userId;
	packet.level = clients[userId].level;
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
	packet.x = npcs[npcId - (NPC_ID_START + 1)].x;
	packet.y = npcs[npcId - (NPC_ID_START + 1)].y;
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
	packet.o_type = O_HUMAN;

	sendPacket(userId, &packet);
}

void sendEnterPacketNpc(int userId, int npcId) {
	sc_packet_enter packet{};
	packet.id = npcId;
	packet.size = sizeof(packet);
	packet.type = S2C_ENTER;
	packet.x = npcs[npcId - (NPC_ID_START + 1)].x;
	packet.y = npcs[npcId - (NPC_ID_START + 1)].y;
	strcpy_s(packet.name, npcs[npcId - (NPC_ID_START + 1)].name);
	packet.o_type = O_ORC;

	sendPacket(userId, &packet);
}

void sendChatPacket(int userId, int chatId, const char* message) {
	sc_packet_chat packet{};
	packet.id = chatId;
	packet.size = sizeof(packet);
	packet.type = S2C_CHAT;
	int length{ WideCharToMultiByte(CP_ACP, 0, (LPCWCH)message, -1, nullptr, 0, nullptr, nullptr) };
	MultiByteToWideChar(CP_ACP, 0, (LPCCH)message, -1, packet.mess, length);
	sendPacket(userId, &packet);
}

void sendChatPacket(int userId, int chatId, const wchar_t* message) {
	sc_packet_chat packet{};
	packet.id = chatId;
	packet.size = sizeof(packet);
	packet.type = S2C_CHAT;
	wcscpy_s(packet.mess, message);
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
		if (isInArea(clients[idCluster].x, clients[idCluster].y, npcs[npcId].x, npcs[npcId].y, VIEW_RADIUS)) {
			npcs[npcId].mtx.lock();
			if (npcs[npcId].viewList.count(idCluster)) {
				npcs[npcId].mtx.unlock();
				bFlags = true;
				sendMovePacketNpc(idCluster, npcId + NPC_ID_START + 1);
			}
			else {
				npcs[npcId].viewList.emplace(idCluster);
				npcs[npcId].mtx.unlock();
				sendEnterPacketNpc(idCluster, npcId + NPC_ID_START + 1);
				bFlags = true;
			}
				
		}
		else {
			npcs[npcId].mtx.lock();
			if (npcs[npcId].viewList.count(idCluster)) {
				npcs[npcId].viewList.erase(idCluster);
				npcs[npcId].mtx.unlock();
				sendLeavePacket(idCluster, npcId + NPC_ID_START + 1);
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
		int ClusterIdx{ idCluster - (NPC_ID_START + 1) };
		if (isInArea(user.x, user.y, npcs[ClusterIdx].x, npcs[ClusterIdx].y, VIEW_RADIUS)) {
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
			if (isInArea(userId, idCluster, VIEW_RADIUS))
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
			if (isInArea(userId, idCluster, VIEW_RADIUS)) {
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
	auto info {dbDevice.getUserInfo(utf8_to_wstring(name))};
	if (info.name == L"ERROR") {
		closesocket(user.socket);
		user.status = EStatus::E_FREE;
	}
	else {
		user.x = info.x;
		user.y = info.y;
		user.level = info.level;
		user.hp = info.hp;
		user.exp = info.exp;

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
		
		std::string s{ user.name };
		s.append(" 님 안녕하세요.");
		sendChatPacket(userId, -64, s.c_str());
	}
}

int API_send_message(lua_State* L) {
	int playerId{ static_cast<int>(lua_tointeger(L, -3)) };
	int objId{ static_cast<int>(lua_tointeger(L, -2)) };
	char* message{ (char*)lua_tostring(L, -1)};
	lua_pop(L, 3);

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
	std::string name{};
	name.reserve(50);
	for (int i = 0; i < NUM_NPC; ++i) {
		npcs[i].x = rand() % WORLD_WIDTH;
		npcs[i].y = rand() % WORLD_HEIGHT;
		npcs[i].id = i + NPC_ID_START + 1;

		auto [secX, secY] {getSectorPos(npcs[i].x, npcs[i].y)};
		npcSector[secX][secY].emplace(npcs[i].id);
		npcs[i].curSectX = secX;
		npcs[i].curSectY = secY;
		npcs[i].moveCount = 3;
		npcs[i].level = rand() % 10 + 1;
		npcs[i].hp = npcs[i].level * 100;
		name.append("LV : ");
		name.append(std::to_string(npcs[i].level));
		strcpy_s(npcs[i].name, name.c_str());
		npcs[i].name;
		//strcpy_s(npcs[i].name, )
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
		name.clear();
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
	dbDevice.setUserInfo(UserInfo{ utf8_to_wstring(user.name), user.x, user.y, user.level, user.exp, user.hp });

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
	case C2S_CHAT: {
		cs_packet_chat* packet = reinterpret_cast<cs_packet_chat*>(buf);
		Client& user{ clients[userId] };
		user.mtx.lock();
		std::unordered_set<int> oldViewList{ clients[userId].viewList };
		user.mtx.unlock();
		for (auto& target : oldViewList)
			sendChatPacket(target, user.id, packet->mess);
		break;
	}
	case C2S_ATTACK: {
		auto curTime{ std::chrono::high_resolution_clock::now() };
		if (std::chrono::duration<float, std::milli>
			(curTime - clients[userId].lastAttackTime).count() > 1000.0f) {
			Client& user{ clients[userId] };
			int x{ clients[userId].x };
			int y{ clients[userId].y };

			std::vector<int> v{};
			// TODO : 근처 섹터도 검색하게 수정해야 함.
			// TODO : 몬스터 사망 및 경험치업 & 렙업 추가
			npcSectorMtx[user.curSectX][user.curSectY].lock();
			v.reserve(npcSector[user.curSectX][user.curSectY].size());
			for (auto& i : npcSector[user.curSectX][user.curSectY] )
				v.emplace_back(i);
			npcSectorMtx[user.curSectX][user.curSectY].unlock();
			for (auto& i : v) {
				int ClusterIdx{ i - (NPC_ID_START + 1) };
				if (isInArea(user.x, user.y, npcs[ClusterIdx].x, npcs[ClusterIdx].y, 2)) {
					std::string s{};
					s.reserve(80);
					s.append(user.name);
					s.append(" 가 공격하여 ");
					s.append(std::to_string(user.level * 20));
					s.append(" 의 데미지를 입혔습니다.");
					sendChatPacket(userId, -64, s.c_str());
					npcs[ClusterIdx].hp -= user.level * 20;
				}
			}
			user.lastAttackTime = std::chrono::high_resolution_clock::now();
		}
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
			packetSize = unsigned char(currentBuffer[0]);
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
			key = key - (NPC_ID_START + 1);
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
				eventQueue.push(EventType{ static_cast<int>(key + NPC_ID_START + 1), exOver->playerId, std::chrono::high_resolution_clock::now() });

			}
			else {
				npcs[key].isActive = false;
				//sendChatPacket(exOver->playerId, key + NPC_ID_START + 1, "BYE");
			}
			delete exOver;
			break;
		}
		
		case EOperation::E_PLAYERMOVE: {
			npcs[key- (NPC_ID_START + 1)].luaMtx.lock();
			lua_State*& L{npcs[key - (NPC_ID_START + 1)].L};
			lua_getglobal(L, "eventPlayerMove");
			lua_pushnumber(L, exOver->playerId);

			if (int error{ lua_pcall(L, 1, 0, 0) }; error) {
				std::cout << lua_tostring(L, -1);
				lua_pop(L, 1);
			}

			npcs[key - (NPC_ID_START + 1)].luaMtx.unlock();
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

	for (int i = 0; i < 4; ++i)
		workers.emplace_back(workerThread);
	workers.emplace_back(timerThread);
	for (auto& th : workers)
		th.join();


}