#pragma once

class InfoManager{
public:
	InfoManager();
	~InfoManager() = default;
	void draw(sf::RenderWindow* window);
	void changeInfo(int exp_, int hp_, int level_);

private:
	int level{}, exp{}, hp{};
	sf::Text drawInfo{};
	sf::Font infoFont{};
	std::wstring currentInfo{};
};

