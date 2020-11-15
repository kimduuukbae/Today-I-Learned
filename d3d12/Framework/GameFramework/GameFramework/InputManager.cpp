#include "stdafx.h"
#include "InputManager.h"

InputManager::InputManager()
{

}

InputManager::~InputManager()
{
	events[true].clear();
	events[false].clear();
}

void InputManager::Init()
{
	events[false].reserve(64);
	events[true].reserve(64);
}

void InputManager::PushEvent(unsigned char key, unsigned char state)
{
	events[state].emplace(key);

	if (state == false)
		events[true].erase(key);
}

void InputManager::PushAxisEvent(unsigned char key, float axis)
{
	axisEvents[key] = axis;
}

void InputManager::ReleaseEvent()
{
	axisEvents.clear();
	events[false].clear();
}
