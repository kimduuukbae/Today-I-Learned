#pragma once

class Object {
private:
	bool m_showing;
	sf::Sprite m_sprite;

	char m_mess[MAX_STR_LEN];
	high_resolution_clock::time_point m_time_out;
	sf::Text m_text;
	sf::Text m_name;
	sf::RenderWindow* window{ nullptr };

	int* gLeft{ nullptr };
	int* gTop{ nullptr };

	float offsetX{ 0.0f };
	float offsetY{ 0.0f };

	int nameOffsetX{ -10 };
	int nameOffsetY{ -15 };
public:
	int m_x, m_y;
	char name[MAX_ID_LEN];
	Object(sf::Texture& t, int x, int y, int x2, int y2) {
		m_showing = false;
		m_sprite.setTexture(t);
		m_sprite.setTextureRect(sf::IntRect(x, y, x2, y2));
		m_time_out = high_resolution_clock::now();
		m_sprite.setScale(1.5f, 1.5f);
		m_text.setFillColor(sf::Color{ 0, 0, 0 });
		m_text.setStyle(sf::Text::Bold);
	}
	Object() {
		m_showing = false;
		m_time_out = high_resolution_clock::now();
	}

	void show(){
		m_showing = true;
	}
	void hide(){
		m_showing = false;
	}

	void a_move(int x, int y) {
		m_sprite.setPosition((float)x, (float)y);
	}
	void setTile(int x, int y) {
		m_sprite.setTextureRect(sf::IntRect{ x, y, 64, 64 });
	}
	void a_draw() {
		window->draw(m_sprite);
	}

	void move(int x, int y) {
		m_x = x;
		m_y = y;
	}
	void draw() {
		if (false == m_showing) return;
		float rx = ((m_x - *gLeft) * 65.0f) + offsetX;
		float ry = ((m_y - *gTop) * 65.0f) + offsetY;
		m_sprite.setPosition(rx, ry);
		window->draw(m_sprite);
		m_name.setPosition(rx + nameOffsetX, ry + nameOffsetY);
		window->draw(m_name);
		if (high_resolution_clock::now() < m_time_out) {
			m_text.setPosition(rx - 10, ry - 46);
			window->draw(m_text);
		}
	}
	void set_name(char str[]) {
		m_name.setString(str);
		m_name.setFillColor(sf::Color(255, 255, 0));
		m_name.setStyle(sf::Text::Bold);
	}
	void add_chat(wchar_t chat[]) {
		m_text.setString(chat);
		m_time_out = high_resolution_clock::now() + 3s;
	}

	sf::Sprite& getSprite() {
		return m_sprite;
	}

	void setWindow(sf::RenderWindow* tWindow) {
		window = tWindow;
	}

	void setWindowOffset(int& left, int& top) {
		gLeft = &left;
		gTop = &top;
	}

	void setFont(sf::Font& font) {
		m_text.setFont(font);
		m_name.setFont(font);
	}

	void setSize(float x, float y) {
		m_sprite.setScale(x, y);
	}

	void setOffset(float x, float y) {
		offsetX = x;
		offsetY = y;
	}

	void nameOffset(float x, float y) {
		nameOffsetX = x;
		nameOffsetY = y;
	}
};

