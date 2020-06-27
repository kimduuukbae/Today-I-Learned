#include "stdafx.h"
#include "inc/fmod.hpp"
#include "ChatManager.h"
#include "InfoManager.h"
#include "Object.h"
#pragma comment(lib, "inc/fmod_vc.lib")

sf::TcpSocket g_socket;

int g_left_x;
int g_top_y;
int g_myid;

sf::RenderWindow* g_window;
sf::Font g_font;
ChatManager allChat{};
InfoManager avartarInfo{};
std::array<std::array<int, 800>, 800> mapTile{};


Object avatar;
unordered_map <int, Object> npcs;

Object white_tile;
Object black_tile;

sf::Texture* board;
sf::Texture* character;
sf::Texture* monster[2];
FMOD::System* pFmod;
FMOD::Channel* ch[2];
FMOD::ChannelGroup* gr[2];
FMOD::Sound* Sound[2];

void client_initialize()
{
	FMOD::System_Create(&pFmod);
	pFmod->init(2, FMOD_INIT_NORMAL, nullptr);
	pFmod->createSound("data/bgm.mp3", FMOD_LOOP_NORMAL, nullptr, &Sound[0]);
	pFmod->createSound("data/attack.wav", FMOD_LOOP_OFF, nullptr, &Sound[1]);
	board = new sf::Texture;
	character = new sf::Texture;
	monster[0] = new sf::Texture;
	monster[1] = new sf::Texture;
	if (false == g_font.loadFromFile("nanum.ttf")) {
		cout << "Font Loading Error!\n";
		while (true);
	}
	board->loadFromFile("data/tile.png");
	character->loadFromFile("data/character.png");
	monster[0]->loadFromFile("data/enemy1.png");
	monster[1]->loadFromFile("data/enemy2.png");
	white_tile = Object{ *board, 5, 5, TILE_WIDTH, TILE_WIDTH };
	black_tile = Object{ *board, 69, 5, TILE_WIDTH, TILE_WIDTH };
	avatar = Object{ *character, 0, 0, 59, 59 };
	avatar.move(4, 4);
	avatar.setFont(g_font);
	avatar.setSize(1.8f, 1.8f);
	avatar.setOffset(-10.0f, -20.0f);
	//init mapTile
	std::ifstream ifs{ "map.txt", ios::binary };
	std::istream_iterator<char> it{ ifs };

	for (int i = 0; i < WORLD_WIDTH; ++i) {
		for (int j = 0; j < WORLD_HEIGHT; ++j) {
			mapTile[i][j] = (*it) - 48;
			++it;
		}
	}
	ifs.close();
}

void client_finish()
{	
	delete board;
	delete character;
}

void ProcessPacket(char* ptr)
{
	static bool first_time = true;
	switch (ptr[1])
	{
	case S2C_LOGIN_OK:
	{
		sc_packet_login_ok* my_packet = reinterpret_cast<sc_packet_login_ok*>(ptr);
		g_myid = my_packet->id;
		avatar.move(my_packet->x, my_packet->y);
		g_left_x = my_packet->x - (SCREEN_WIDTH / 2);
		g_top_y = my_packet->y - (SCREEN_HEIGHT / 2);
		avartarInfo.changeInfo(my_packet->exp, my_packet->hp, my_packet->level);
		avatar.show();
	}
	break;

	case S2C_ENTER:
	{
		sc_packet_enter* my_packet = reinterpret_cast<sc_packet_enter*>(ptr);
		int id = my_packet->id;

		if (id == g_myid) {
			avatar.move(my_packet->x, my_packet->y);
			//g_left_x = my_packet->x - (SCREEN_WIDTH / 2);
			//g_top_y = my_packet->y - (SCREEN_HEIGHT / 2);
			avatar.show();
		}
		else {
			if (id < NPC_ID_START)
				npcs[id] = Object{ *character, 0, 0, 59, 59 };
			else {
				if (my_packet->o_type == 1)
					npcs[id] = Object{ *monster[0], 0, 0, 64, 65 };
				else if (my_packet->o_type == 2)
					npcs[id] = Object{ *monster[1], 0, 0, 64, 65 };
			}
				
			strcpy_s(npcs[id].name, my_packet->name);
			npcs[id].set_name(my_packet->name);
			npcs[id].move(my_packet->x, my_packet->y);
			npcs[id].show();
			npcs[id].setFont(g_font);
			npcs[id].setWindow(g_window);
			npcs[id].setWindowOffset(g_left_x, g_top_y);
			npcs[id].setOffset(-15.0f, -25.0f);
			npcs[id].setSize(1.8f, 1.8f);
			npcs[id].nameOffset(20, -15);
		}
	}
	break;
	case S2C_MOVE:
	{
		sc_packet_move* my_packet = reinterpret_cast<sc_packet_move*>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid) {
			int right{ my_packet->x - avatar.m_x };
			int up{ my_packet->y - avatar.m_y };
			if (right == 1)
				avatar.getSprite().setTextureRect(sf::IntRect{0, 118, 59, 59});
			else if (right == -1)
				avatar.getSprite().setTextureRect(sf::IntRect{ 0, 59, 59, 59 });
			else if (up == -1)
				avatar.getSprite().setTextureRect(sf::IntRect{0 , 177, 59, 59 });
			else if (up == 1)
				avatar.getSprite().setTextureRect(sf::IntRect{ 0, 0, 59, 59 });

			
			avatar.move(my_packet->x, my_packet->y);
			g_left_x = my_packet->x - (SCREEN_WIDTH / 2);
			g_top_y = my_packet->y - (SCREEN_HEIGHT / 2);
		}
		else {
			if (0 != npcs.count(other_id)) {
				int right{ my_packet->x - npcs[other_id].m_x };
				int up{ my_packet->y - npcs[other_id].m_y };
				
				if (right == 1)
					npcs[other_id].getSprite().setTextureRect(sf::IntRect{ 0, 130, 64, 65 });
				else if (right == -1)
					npcs[other_id].getSprite().setTextureRect(sf::IntRect{ 0, 65, 64, 65 });
				else if (up == -1)
					npcs[other_id].getSprite().setTextureRect(sf::IntRect{ 0, 195, 64, 65 });
				else if (up == 1)
					npcs[other_id].getSprite().setTextureRect(sf::IntRect{ 0, 0, 64, 65 });

				npcs[other_id].move(my_packet->x, my_packet->y);
			}
		}
	}
	break;

	case S2C_LEAVE:
	{
		sc_packet_leave* my_packet = reinterpret_cast<sc_packet_leave*>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid) {
			avatar.hide();
		}
		else {
			if (0 != npcs.count(other_id))
				npcs[other_id].hide();
		}
	}
	break;
	case S2C_CHAT: {
		sc_packet_chat* my_packet = reinterpret_cast<sc_packet_chat*>(ptr);
		int o_id = my_packet->id;
		std::wstring s{};
		
		if (o_id == -64) {
			s.append(L"<system> : ");
			s.append(my_packet->mess);
			allChat.pushText(s.c_str());
		}
		else {
			if (o_id == g_myid) {
				s.append(utf8_to_wstring(avatar.name));
				avatar.add_chat(my_packet->mess);
			}
			else if (npcs.count(o_id) != 0) {
				s.append(utf8_to_wstring(npcs[o_id].name));
				npcs[o_id].add_chat(my_packet->mess);
			}
			s.append(L" : ");
			s.append(my_packet->mess);
			allChat.pushText(s.c_str());
		}
	}
	break;
	case S2C_STAT_CHANGE: {
		sc_packet_stat_change* my_packet = reinterpret_cast<sc_packet_stat_change*>(ptr);
		avartarInfo.changeInfo(my_packet->exp, my_packet->hp, my_packet->level);
	}
	break;
	case S2C_LOGIN_FAIL: {
		MessageBox(g_window->getSystemHandle(), "이미 접속중인 ID 입니다.", 0, 0);
		g_window->close();
	}
	break;
	default:
		printf("Unknown PACKET type [%d]\n", ptr[1]);

	}
}

void process_data(char* net_buf, size_t io_byte)
{
	char* ptr = net_buf;
	static size_t in_packet_size = 0;
	static size_t saved_packet_size = 0;
	static char packet_buffer[BUF_SIZE];

	while (0 != io_byte) {
		if (0 == in_packet_size) in_packet_size = unsigned char(ptr[0]);
		if (io_byte + saved_packet_size >= in_packet_size) {
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			ProcessPacket(packet_buffer);
			ptr += in_packet_size - saved_packet_size;
			io_byte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, io_byte);
			saved_packet_size += io_byte;
			io_byte = 0;
		}
	}
}

void client_main()
{
	char net_buf[BUF_SIZE];
	size_t	received;

	auto recv_result = g_socket.receive(net_buf, BUF_SIZE, received);
	if (recv_result == sf::Socket::Error)
	{
		wcout << L"Recv 에러!";
		while (true);
	}

	if (recv_result == sf::Socket::Disconnected)
	{
		wcout << L"서버 접속 종료.";
		g_window->close();
	}

	if (recv_result != sf::Socket::NotReady)
		if (received > 0) process_data(net_buf, received);

	for (int i = 0; i < SCREEN_WIDTH; ++i)
		for (int j = 0; j < SCREEN_HEIGHT; ++j)
		{
			int tile_x = i + g_left_x;
			int tile_y = j + g_top_y;
			if ((tile_x < 0) || (tile_y < 0)) continue;

			black_tile.setTile(mapTile[tile_x][tile_y] * 64, 0);
			black_tile.a_move(TILE_WIDTH * i + 7, TILE_WIDTH * j + 7);
			black_tile.a_draw();
		}
	avatar.draw();
	//	for (auto &pl : players) pl.draw();
	for (auto& npc : npcs) npc.second.draw();
	sf::Text text;
	text.setFont(g_font);
	char buf[100];
	sprintf_s(buf, "(%d, %d)", avatar.m_x, avatar.m_y);
	text.setString(buf);
	g_window->draw(text);
	allChat.draw(g_window);
	avartarInfo.draw(g_window);
}

void send_packet(void* packet)
{
	char* p = reinterpret_cast<char*>(packet);
	size_t sent;
	g_socket.send(p, unsigned char(p[0]), sent);
}

void send_chat_packet(const wstring& text) {
	cs_packet_chat m_packet{};
	wcscpy_s(m_packet.mess, text.c_str());
	m_packet.size = sizeof(m_packet);
	m_packet.type = C2S_CHAT;
	send_packet(&m_packet);
}

void send_move_packet(unsigned char dir)
{
	int x{ avatar.m_x };
	int y{ avatar.m_y };

	switch (dir) {
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
		cs_packet_move m_packet;
		m_packet.type = C2S_MOVE;
		m_packet.size = sizeof(m_packet);
		m_packet.direction = dir;
		send_packet(&m_packet);
}

void send_attack_packet() {
	cs_packet_attack m_packet;
	m_packet.size = sizeof(m_packet);
	m_packet.type = C2S_ATTACK;
	send_packet(&m_packet);
}

int main()
{
	wcout.imbue(locale("korean"));
	sf::Socket::Status status = g_socket.connect("127.0.0.1", SERVER_PORT);
	g_socket.setBlocking(false);

	if (status != sf::Socket::Done) {
		wcout << L"서버와 연결할 수 없습니다.\n";
		while (true);
	}

	client_initialize();

	wcout << L"아이디를 입력해주세요:" << endl;
	string id{};
	cin >> id;
	cs_packet_login l_packet;
	l_packet.size = sizeof(l_packet);
	l_packet.type = C2S_LOGIN;
	strcpy_s(l_packet.name, id.c_str());
	strcpy_s(avatar.name, l_packet.name);
	avatar.set_name(l_packet.name);
	send_packet(&l_packet);
	pFmod->playSound(Sound[0], gr[0], false, &ch[0]);
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH , WINDOW_HEIGHT), "2D CLIENT");
	g_window = &window;

	avatar.setWindow(g_window);
	avatar.setWindowOffset(g_left_x, g_top_y);
	black_tile.setWindow(g_window);
	black_tile.setWindowOffset(g_left_x, g_top_y);
	white_tile.setWindow(g_window);
	white_tile.setWindowOffset(g_left_x, g_top_y);



	sf::View view = g_window->getView();
	view.zoom(2.0f);
	view.move(SCREEN_WIDTH * TILE_WIDTH / 4, SCREEN_HEIGHT * TILE_WIDTH / 4);
	g_window->setView(view);
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::KeyPressed) {
				int p_type = -1;
				switch (event.key.code) {
				case sf::Keyboard::Left:
					send_move_packet(D_LEFT);
					break;
				case sf::Keyboard::Right:
					send_move_packet(D_RIGHT);
					break;
				case sf::Keyboard::Up:
					send_move_packet(D_UP);
					break;
				case sf::Keyboard::Down:
					send_move_packet(D_DOWN);
					break;
				case sf::Keyboard::Escape:
					window.close();
					break;
				case sf::Keyboard::LControl:
					send_attack_packet();
					pFmod->playSound(Sound[1], gr[1], false, &ch[1]);
					break;
				case sf::Keyboard::Enter:
					if (!allChat.isEnableChat()) 
						allChat.setEnableChat(true);
					else {
						if (wstring& chat{ allChat.getCurrentTyping() }; chat.size()){
							send_chat_packet(chat);
							allChat.clearTyping();
							allChat.setEnableChat(false);
						}
					}
						
					break;
				}
			}
			else if (event.type == sf::Event::TextEntered && event.key.code != 13)
				if (allChat.isEnableChat()) {
					if (event.key.code == 8)
						allChat.popTyping();
					else
						allChat.typing(event.text.unicode);
				}
		}

		window.clear();
		client_main();
		window.display();
	}
	client_finish();

	return 0;
}