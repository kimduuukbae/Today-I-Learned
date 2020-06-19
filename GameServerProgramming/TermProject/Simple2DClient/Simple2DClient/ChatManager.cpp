#include "stdafx.h"
#include "ChatManager.h"

ChatManager::ChatManager(){
	chatRoomTex.loadFromFile("data/chatroom.png");
	chatFont.loadFromFile("nanum.ttf");
	chatRoom.setTexture(chatRoomTex);
	chatRoom.setPosition(0, WINDOW_HEIGHT * 2 - (216*2));
	chatRoom.setTextureRect(sf::IntRect{ 0,0,260,216 });
	chatRoom.setScale(5.1f, 2.0f);
	chatRoom.setColor(sf::Color{255, 255, 255, 127});
	
	currentText.setFont(chatFont);
	currentText.setPosition(110, (WINDOW_HEIGHT * 2 - 50));
	currentText.setFillColor(sf::Color{ 0,0,0 });
	currentText.setStyle(sf::Text::Bold);

	currentTyping.reserve(80);
}

void ChatManager::draw(sf::RenderWindow* window){
	window->draw(chatRoom);
	size_t listSize{ chatList.size() };
	auto it{ chatList.begin() };
	for (int i = 0; i < listSize; ++i, ++it) {
		it->setPosition(20, (WINDOW_HEIGHT*2 - 96) - (i * 32));
		window->draw(*it);
	}
	if (enable) 
		window->draw(currentText);
}

void ChatManager::pushText(const wchar_t* chat){
	if (chatList.size() > 10) 
		chatList.pop_back();
	
	chatList.emplace_front();
	chatList.front().setString(chat);
	chatList.front().setFont(chatFont);
	chatList.front().setStyle(sf::Text::Bold);
	chatList.front().setFillColor(sf::Color{ 0,0,0 });
}

void ChatManager::setEnableChat(bool bFlag){
	enable = bFlag;
}

bool ChatManager::isEnableChat(){
	return enable;
}

void ChatManager::typing(wchar_t character){
	if (currentTyping.size() < 80) {
		currentTyping.push_back(character);
		currentText.setString(currentTyping.c_str());
	}
}

void ChatManager::popTyping(){
	if (currentTyping.size()) {
		currentTyping.pop_back();
		currentText.setString(currentTyping.c_str());
	}
}

void ChatManager::clearTyping(){
	currentTyping.clear();
	currentText.setString(currentTyping.c_str());
}

std::wstring& ChatManager::getCurrentTyping(){
	return currentTyping;
}


