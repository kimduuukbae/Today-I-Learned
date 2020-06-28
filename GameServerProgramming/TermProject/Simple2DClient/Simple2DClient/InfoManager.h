#pragma once

class InfoManager{
public:
	InfoManager();
	~InfoManager() = default;
	void draw(sf::RenderWindow* window);
	void changeInfo(int exp_, int hp_, int level_);
	inline std::tuple<int, int, int> getInfo() {
		return { level, exp, hp };
	}
private:
	int level{}, exp{}, hp{};
	sf::Text drawInfo{};
	sf::Font infoFont{};
	std::wstring currentInfo{};
};

