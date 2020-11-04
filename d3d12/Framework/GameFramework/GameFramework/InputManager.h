#pragma once

#include "Singleton.h"

using KEY = unsigned char;
using STATE = unsigned char;

class InputManager : public Singleton<InputManager>
{
public:
	InputManager();
	~InputManager();

	void Init();

	[[nodiscard]] const std::unordered_map<KEY, STATE>& GetEvents()
	{
		return events;
	}

private:
	friend class FrameworkApp;

	void PushEvent(unsigned char key, unsigned char state);
	void ReleaseEvent();

	std::unordered_map<KEY, STATE> events;
	std::array<unsigned char, 256> keys{};
};

