#include "stdafx.h"
#include "inc/fmod.hpp"
#include "ChatManager.h"
#include "InfoManager.h"
#pragma comment(lib, "inc/fmod_vc.lib")


// TODO : 로그인창 추가
using namespace std;
using namespace chrono;

sf::TcpSocket g_socket;

int g_left_x;
int g_top_y;
int g_myid;

sf::RenderWindow* g_window;
sf::Font g_font;
ChatManager allChat{};
InfoManager avartarInfo{};
class OBJECT {
private:
	bool m_showing;
	sf::Sprite m_sprite;

	char m_mess[MAX_STR_LEN];
	high_resolution_clock::time_point m_time_out;
	sf::Text m_text;
	sf::Text m_name;

public:
	int m_x, m_y;
	char name[MAX_ID_LEN];
	OBJECT(sf::Texture& t, int x, int y, int x2, int y2) {
		m_showing = false;
		m_sprite.setTexture(t);
		m_sprite.setTextureRect(sf::IntRect(x, y, x2, y2));
		m_sprite.setScale(sf::Vector2f(1.5f, 1.5f));
		m_time_out = high_resolution_clock::now();
		m_text.setFillColor(sf::Color{ 0, 0, 0 });
		m_text.setStyle(sf::Text::Bold);
	}
	OBJECT() {
		m_showing = false;
		m_time_out = high_resolution_clock::now();
	}
	void show()
	{
		m_showing = true;
	}
	void hide()
	{
		m_showing = false;
	}

	void a_move(int x, int y) {
		m_sprite.setPosition((float)x, (float)y);
	}

	void a_draw() {
		g_window->draw(m_sprite);
	}

	void move(int x, int y) {
		m_x = x;
		m_y = y;
	}
	void draw() {
		if (false == m_showing) return;
		float rx = (m_x - g_left_x) * 65.0f;
		float ry = (m_y - g_top_y) * 65.0f;
		m_sprite.setPosition(rx, ry);
		g_window->draw(m_sprite);
		m_name.setPosition(rx - 10, ry - 10);
		g_window->draw(m_name);
		if (high_resolution_clock::now() < m_time_out) {
			m_text.setPosition(rx - 10, ry - 46);
			g_window->draw(m_text);
		}
	}
	void set_name(char str[]) {
		m_name.setFont(g_font);
		m_name.setString(str);
		m_name.setFillColor(sf::Color(255, 255, 0));
		m_name.setStyle(sf::Text::Bold);
	}
	void add_chat(wchar_t chat[]) {
		m_text.setFont(g_font);
		m_text.setString(chat);
		m_time_out = high_resolution_clock::now() + 3s;
	}

	sf::Sprite& getSprite() {
		return m_sprite;
	}
};

OBJECT avatar;
unordered_map <int, OBJECT> npcs;

OBJECT white_tile;
OBJECT black_tile;

sf::Texture* board;
sf::Texture* character;
sf::Texture* monster;
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
	monster = new sf::Texture;
	if (false == g_font.loadFromFile("nanum.ttf")) {
		cout << "Font Loading Error!\n";
		while (true);
	}
	board->loadFromFile("data/tile.png");
	character->loadFromFile("data/character.png");
	monster->loadFromFile("data/monster.png");
	white_tile = OBJECT{ *board, 5, 5, TILE_WIDTH, TILE_WIDTH };
	black_tile = OBJECT{ *board, 69, 5, TILE_WIDTH, TILE_WIDTH };
	avatar = OBJECT{ *character, 0, 0, 48, 48 };
	avatar.move(4, 4);
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
				npcs[id] = OBJECT{ *character, 0, 0, 48, 48 };
			else
				npcs[id] = OBJECT{ *monster, 128, 0, 44, 48 };
			strcpy_s(npcs[id].name, my_packet->name);
			npcs[id].set_name(my_packet->name);
			npcs[id].move(my_packet->x, my_packet->y);
			npcs[id].show();
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
				avatar.getSprite().setTextureRect(sf::IntRect{440, 0, 48, 48});
			else if (right == -1)
				avatar.getSprite().setTextureRect(sf::IntRect{ 120, 0, 48, 48 });
			else if (up == -1)
				avatar.getSprite().setTextureRect(sf::IntRect{ 240, 0, 48, 48 });
			else if (up == 1)
				avatar.getSprite().setTextureRect(sf::IntRect{ 0, 0, 48, 48 });

			
			avatar.move(my_packet->x, my_packet->y);
			g_left_x = my_packet->x - (SCREEN_WIDTH / 2);
			g_top_y = my_packet->y - (SCREEN_HEIGHT / 2);
		}
		else {
			if (0 != npcs.count(other_id)) {
				int right{ my_packet->x - npcs[other_id].m_x };
				int up{ my_packet->y - npcs[other_id].m_y };
				
				if (right == 1)
					npcs[other_id].getSprite().setTextureRect(sf::IntRect{ 440, 0, 44, 48 });
				else if (right == -1)
					npcs[other_id].getSprite().setTextureRect(sf::IntRect{ 126, 0, 44, 48 });
				else if (up == -1)
					npcs[other_id].getSprite().setTextureRect(sf::IntRect{ 246, 0, 44, 48 });
				else if (up == 1)
					npcs[other_id].getSprite().setTextureRect(sf::IntRect{ 0, 0, 44, 48 });

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
			s.append(L"서버 : ");
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
			//if (((tile_x + tile_y) % 2) == 0) {
			if (((tile_x / 3 + tile_y / 3) % 2) == 0) {
				white_tile.a_move(TILE_WIDTH * i + 7, TILE_WIDTH * j + 7);
				white_tile.a_draw();
			}
			else
			{
				black_tile.a_move(TILE_WIDTH * i + 7, TILE_WIDTH * j + 7);
				black_tile.a_draw();
			}
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