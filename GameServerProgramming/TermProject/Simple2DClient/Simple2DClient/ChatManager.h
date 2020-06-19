#pragma once

class ChatManager {
public:
	ChatManager();
	~ChatManager() = default;
	void draw(sf::RenderWindow* window);
	void pushText(const wchar_t* chat);
	void setEnableChat(bool bFlag);
	bool isEnableChat();
	void typing(wchar_t character);
	void popTyping();
	void clearTyping();
	std::wstring& getCurrentTyping();
private:
	sf::Font chatFont{};
	std::deque<sf::Text> chatList{};
	sf::Sprite chatRoom{};
	sf::Texture chatRoomTex{};

	sf::Text currentText{};
	std::wstring currentTyping{};
	bool enable{ false };
};

