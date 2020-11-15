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

	[[nodiscard]] const std::array<std::unordered_set<KEY>, 2>& GetEvents()
	{
		return events;
	}

	[[nodiscard]] const std::unordered_map<KEY, float>& GetAxisEvents()
	{
		return axisEvents;
	}

private:
	friend class FrameworkApp;

	void PushEvent(unsigned char key, unsigned char state);
	void PushAxisEvent(unsigned char key, float axis);
	void ReleaseEvent();

	std::array<std::unordered_set<KEY>, 2> events;
	std::unordered_map<KEY, float> axisEvents;
};

