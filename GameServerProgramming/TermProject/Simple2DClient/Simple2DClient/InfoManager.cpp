#include "stdafx.h"
#include "InfoManager.h"

InfoManager::InfoManager(){
	infoFont.loadFromFile("nanum.ttf");
	currentInfo.reserve(80);
	drawInfo.setFont(infoFont);
	drawInfo.setFillColor(sf::Color{ 255, 0, 0 });
	drawInfo.setPosition(10, 40);
	drawInfo.setStyle(sf::Text::Bold);
}

void InfoManager::draw(sf::RenderWindow * window){
	window->draw(drawInfo);
}

void InfoManager::changeInfo(int exp_, int hp_, int level_) {
	level = level_;
	exp = exp_;
	hp = hp_;
	std::to_wstring(exp);
	std::to_wstring(hp);

	currentInfo.clear();

	currentInfo.append(L"LEVEL : ");
	currentInfo.append(std::to_wstring(level));
	currentInfo.append(L"\nEXP : ");
	currentInfo.append(std::to_wstring(exp));
	currentInfo.append(L"\nHP : ");
	currentInfo.append(std::to_wstring(hp));

	drawInfo.setString(currentInfo);
}
